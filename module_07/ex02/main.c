#include "uart.h"
#include "utils.h"

#include <avr/eeprom.h>
#include <stddef.h>
#include <stdint.h>

#define BACKSPACE 0x7f
#define ERASE_CHAR_TERM "\b \b"
#define MAX_INPUT_SIZE 0x100

#define NB_CONFIG 4 // MUST STATYS A POWER OF 2
#define CONFIG_TAG_SIZE 32
#define LEN_NUMBER 11

#define NODE_COMMAND_STATUS_STR "STATUS"
#define NODE_COMMAND_SET_ID_STR "SET_ID"
#define NODE_COMMAND_SET_PRIORITY_STR "SET_PRIO"
#define NODE_COMMAND_SET_TAG_STR "SET_TAG"
#define NODE_COMMAND_SET_SLOT_STR "SET_SLOT"
#define NODE_COMMAND_FACTORY_RESET_STR "FACTORY_RESET"
#define EEPROM_ERR_MSG_CRITICAL "CRITICAL EEPROM FAILURE."
#define EEPROM_ERR_MSG_DETECTED_CORRUPTION "Corruption detected."
#define EEPROM_MSG_RELOCATION "Relocating config to slot "
#define EPPROM_ERR_MSG_INTEGRETY_CHECK "CRITICAL: Data corruption detected!"
#define EEPROM_MSG_NODE_UNCONFIGURED "Node unconfigured"
#define LEN_LITERAL_STR(s) (sizeof(s) - 1)
#define MAGIC_NUMBER 0xDEEDBEEF
#define pow2_mod(numerator, modulus) ((numerator) & ((modulus) - 1))
typedef uint8_t (*parse_func)(char *arg, void *output, uint16_t *len);

#define FIELD_SIZE(type, field) sizeof(((type *) 0)->field)

typedef struct node_config
{
    char     tag[CONFIG_TAG_SIZE + 1];
    uint32_t magic_number;
    uint32_t id;
    uint16_t crc;
    int16_t  priority;
    uint32_t version;
} node_config;

#define ADDR_NODE_CONFIG(i) (i * sizeof(node_config))

uint16_t curr_slot;
#define GET_CURRENT_NODE_CONFIG (ADDR_NODE_CONFIG(curr_slot))

typedef enum node_command
{
    NODE_COMMAND_STATUS,
    NODE_COMMAND_SET_ID,
    NODE_COMMAND_SET_PRIORITY,
    NODE_COMMAND_SET_TAG,
    NODE_COMMAND_FACTORY_RESET,
    NODE_COMMAND_UNKNOWN
} node_command;

uint8_t check_command_with_args(char *command, char *command_to_match, uint16_t len_command_to_match, char **arg)
{
    if (str_cmp_n(command, command_to_match, len_command_to_match) == 0 && command[len_command_to_match] == ' ')
    {
        *arg = command + len_command_to_match + 1;
        return 1;
    }
    return 0;
}

node_command parse_command(char *command, char **arg)
{
    if (str_cmp(command, NODE_COMMAND_STATUS_STR) == 0)
    {
        *arg = NULL;
        return NODE_COMMAND_STATUS;
    }
    if (check_command_with_args(command, NODE_COMMAND_SET_ID_STR, LEN_LITERAL_STR(NODE_COMMAND_SET_ID_STR), arg))
        return NODE_COMMAND_SET_ID;

    if (check_command_with_args(command, NODE_COMMAND_SET_PRIORITY_STR, LEN_LITERAL_STR(NODE_COMMAND_SET_PRIORITY_STR), arg))
        return NODE_COMMAND_SET_PRIORITY;

    if (check_command_with_args(command, NODE_COMMAND_SET_TAG_STR, LEN_LITERAL_STR(NODE_COMMAND_SET_TAG_STR), arg))
        return NODE_COMMAND_SET_TAG;

    if (str_cmp(command, NODE_COMMAND_FACTORY_RESET_STR) == 0)
        return NODE_COMMAND_FACTORY_RESET;

    return NODE_COMMAND_UNKNOWN;
}

uint16_t getline_and_send(char *buffer, uint16_t max_len)
{
    uint16_t i = 0;
    while (1)
    {
        char c = uart_rx_blocking_get_byte();
        if ((c != '\r' && c != BACKSPACE) && i >= max_len)
        {
            while (i > 0)
            {
                uart_tx_send_str(ERASE_CHAR_TERM);
                i--;
            }
        }
        buffer[i] = c;
        switch (c)
        {
        case '\r':
            uart_tx_send_str("\r\n");
            goto end;
        case BACKSPACE:
            if (i == 0)
                continue;
            i -= 1;
            uart_tx_send_str(ERASE_CHAR_TERM);
            continue;
        default:
            uart_tx_send_byte(c);
            break;
        }
        i++;
    }
end:
    buffer[i] = 0;
    return i;
}

static  uint16_t
_crc16_update(uint16_t __crc, uint8_t __data)
{
	uint8_t __tmp;
	uint16_t __ret;

	__asm__ __volatile__ (
		"eor %A0,%2" "\n\t"
		"mov %1,%A0" "\n\t"
		"swap %1" "\n\t"
		"eor %1,%A0" "\n\t"
		"mov __tmp_reg__,%1" "\n\t"
		"lsr %1" "\n\t"
		"lsr %1" "\n\t"
		"eor %1,__tmp_reg__" "\n\t"
		"mov __tmp_reg__,%1" "\n\t"
		"lsr %1" "\n\t"
		"eor %1,__tmp_reg__" "\n\t"
		"andi %1,0x07" "\n\t"
		"mov __tmp_reg__,%A0" "\n\t"
		"mov %A0,%B0" "\n\t"
		"lsr %1" "\n\t"
		"ror __tmp_reg__" "\n\t"
		"ror %1" "\n\t"
		"mov %B0,__tmp_reg__" "\n\t"
		"eor %A0,%1" "\n\t"
		"lsr __tmp_reg__" "\n\t"
		"ror %1" "\n\t"
		"eor %B0,__tmp_reg__" "\n\t"
		"eor %A0,%1"
		: "=r" (__ret), "=d" (__tmp)
		: "r" (__data), "0" (__crc)
		: "r0"
	);
	return __ret;
}


uint16_t update_crc(uint16_t crc, void *data, uint16_t len)
{
    unsigned char *p = data;
    for (uint16_t i = 0; i < len; i++)
    {
        crc = _crc16_update(crc, p[i]);
    }

    return crc;
}

uint16_t calcul_crc(node_config *config)
{
    uint16_t crc = 0;
    crc          = update_crc(crc, &config->id, sizeof(config->id));
    crc          = update_crc(crc, &config->priority, sizeof(config->priority));
    crc          = update_crc(crc, config->tag, CONFIG_TAG_SIZE + 1);
    return crc;
}

uint8_t update_config_eeprom(uint16_t slot, uint8_t offset_field, const void *data, uint16_t len)
{

    uint16_t       addr_field     = ADDR_NODE_CONFIG(slot) + offset_field;
    uint16_t       end_addr_field = addr_field + len;
    const uint8_t *src            = data;
    for (; addr_field < end_addr_field; addr_field++)
    {
        uint8_t b = *src;
        eeprom_update_byte((uint8_t *) addr_field, b);
        if (eeprom_read_byte((const uint8_t *) addr_field) != b)
            return 0;
        src++;
    }

    return 1;
}

void node_config_print(node_config *config)
{
    char NUM_BUFFER[LEN_NUMBER + 1];
    uart_tx_send_str("Node ID: ");
    unsigned_to_str(config->id, NUM_BUFFER);
    uart_tx_send_str_with_nl(NUM_BUFFER);

    uart_tx_send_str("Priority: ");
    signed_to_str(config->priority, NUM_BUFFER);
    uart_tx_send_str_with_nl(NUM_BUFFER);

    uart_tx_send_str("Slot: ");
    unsigned_to_str(curr_slot, NUM_BUFFER);
    uart_tx_send_str_with_nl(NUM_BUFFER);

    uart_tx_send_str("Tag: \"");
    const uint8_t *addr_tag = (const uint8_t *) config->tag;
    for (uint8_t i = 0; addr_tag[i]; i++)
    {
        uart_tx_send_byte(addr_tag[i]);
    }
    uart_tx_send_str_with_nl("\"");
}

void init_node_config_from_slot(node_config *config, uint8_t slot)
{
    eeprom_read_block(config, (void *) ADDR_NODE_CONFIG(slot), sizeof(*config));
}

void send_err_msg(const char *prefix, const char *msg)
{
    uart_tx_send_str(prefix);
    uart_tx_send_str_with_nl(msg);
}

void do_status()
{
    uint16_t found = -1;
    uint16_t i     = curr_slot;
    do
    {
        uint32_t magic_number = eeprom_read_dword((void *) ADDR_NODE_CONFIG(i) + offsetof(node_config, magic_number));
        if (magic_number == MAGIC_NUMBER)
        {
            curr_slot = i;
            found     = i;
            break;
        }
        i = pow2_mod(i + 1, NB_CONFIG);
    } while (i != curr_slot);

    if (found == (uint16_t) -1)
    {
        uart_tx_send_str_with_nl(EEPROM_MSG_NODE_UNCONFIGURED);
        return;
    }

    node_config config;
    init_node_config_from_slot(&config, curr_slot);
    uint16_t crc = calcul_crc(&config);

    if (crc != config.crc)
    {
        uart_tx_send_str_with_nl(EPPROM_ERR_MSG_INTEGRETY_CHECK);
        return;
    }

    node_config_print(&config);
}

void do_factory_reset()
{
    uint8_t corrupt_byte = 0;
    if (!update_config_eeprom(curr_slot, offsetof(node_config, magic_number), &corrupt_byte, sizeof(corrupt_byte)))
    {
        uart_tx_send_str_with_nl("FACTORY RESET COULD NOT SUCCEED");
    }
}

uint8_t parse_id(char *arg, void *output, uint16_t *len)
{
    if (arg[0] == '-')
    {
        send_err_msg("Invalid id (only positive id allowed): ", arg);
        return 0;
    }

    *len = FIELD_SIZE(node_config, id);
    const char *end;
    uint32_t id = dec_str_to_unsigned(arg, &end);
    if (arg == end || *end != '\0')
    {
        send_err_msg("INVALID ID (only 0-9 char allowed): ", arg);
        return 0;
    }

    mem_copy(output, &id, sizeof(id));
    return 1;
}

uint8_t parse_priority(char *arg, void *output, uint16_t *len)
{
    *len = FIELD_SIZE(node_config, priority);
    const char *end;
    int32_t priority = dec_str_to_signed(arg, &end);
    if (arg == end || *end != '\0' || priority < INT16_MIN || priority > INT16_MAX)
    {
        send_err_msg("INVALID PRIORITY: ", arg);
        return 0;
    }
    int16_t value = (int16_t) priority;
    mem_copy(output, &value, FIELD_SIZE(node_config, priority));
    return 1;
}

uint8_t char_is_alnum(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}

uint8_t parse_tag(char *arg, void *output, uint16_t *len)
{
    if (arg[0] != '"')
    {
        send_err_msg("INVALID TAG (missing opennig quote): ", arg);
        return 0;
    }

    uint8_t i = 0;
    arg++;
    while (arg[i] != 0 && arg[i] != '"')
    {
        if (!char_is_alnum(arg[i]) && arg[i] != '_' && arg[i] != '-')
        {
            send_err_msg("INVALID TAG (A-Z, a-z, 0-9, _, -): ", arg - 1);
            return 0;
        }
        if (i >= CONFIG_TAG_SIZE)
        {
            send_err_msg("INVALID TAG (too long): ", arg - 1);
            return 0;
        }
        i++;
    }

    if (arg[i] == 0)
    {
        send_err_msg("Invalid tag (missing closing quote): ", arg - 1);
        return 0;
    }

    if (arg[i] == '"' && arg[i + 1] != 0)
    {
        send_err_msg("Invalid tag (quote is not last): ", arg - 1);
        return 0;
    }
    arg[i] = 0;
    *len   = CONFIG_TAG_SIZE + 1;
    mem_copy(output, arg, *len);
    return 1;
}

uint8_t node_copy_config_to_slot(uint8_t to, node_config *src)
{
    return update_config_eeprom(to, 0, src, sizeof(*src));
}

void node_save_data(char *arg, uint8_t offset_field, parse_func parser)
{
    uint16_t len_data;
    char     data_buffer[CONFIG_TAG_SIZE + 1];

    if (!parser(arg, data_buffer, &len_data))
        return;

    node_config config;
    init_node_config_from_slot(&config, curr_slot);
    if (config.magic_number != MAGIC_NUMBER)
        mem_set(&config, 0, sizeof(config));
    config.magic_number = MAGIC_NUMBER;
    mem_copy(((char *) (&config)) + offset_field, data_buffer, len_data);
    config.crc = calcul_crc(&config);
    uint8_t success;
#ifdef TRIGGER_RELOC
    success = 0;
#else
    success = node_copy_config_to_slot(curr_slot, &config);
#endif
    if (!success)
    {
        do_factory_reset();
        uart_tx_send_str_with_nl(EEPROM_ERR_MSG_DETECTED_CORRUPTION);
        for (uint8_t i = pow2_mod(curr_slot + 1, NB_CONFIG); i != curr_slot; i = pow2_mod(i + 1, NB_CONFIG))
        {
            uart_tx_send_str(EEPROM_MSG_RELOCATION);
            signed_to_str(i, data_buffer);
            uart_tx_send_str(data_buffer);
            uart_tx_send_str("... ");

            if (node_copy_config_to_slot(i, &config))
            {
                uart_tx_send_str_with_nl("Success");
                uart_tx_send_str_with_nl("Done.");
                curr_slot = i;
                return;
            }
            uart_tx_send_str_with_nl("Fail");
        }
        uart_tx_send_str_with_nl(EEPROM_ERR_MSG_CRITICAL);
    }
}

void do_node_command(node_command command, const char *cmd, char *arg)
{
    switch (command)
    {
    case NODE_COMMAND_STATUS:
        do_status();
        break;

    case NODE_COMMAND_SET_ID:
        node_save_data(arg, offsetof(node_config, id), parse_id);
        break;

    case NODE_COMMAND_SET_PRIORITY:
        node_save_data(arg, offsetof(node_config, priority), parse_priority);
        break;

    case NODE_COMMAND_SET_TAG:
        node_save_data(arg, offsetof(node_config, tag), parse_tag);
        break;

    case NODE_COMMAND_FACTORY_RESET:
        do_factory_reset();
        break;

    default:
        uart_tx_send_str("Unknown command: ");
        uart_tx_send_str(cmd);
        uart_tx_send_str("\r\n");
        break;
    }
}

void set_config_addr()
{
    node_config config;
    for (uint8_t i = 0; i < NB_CONFIG; i++)
    {
        init_node_config_from_slot(&config, i);
        if (config.magic_number == MAGIC_NUMBER && calcul_crc(&config) == config.crc)
        {
            curr_slot = i;
            break;
        }
    }
}

int main()
{
    uart_init_tx_rx(SENDER | RECEIVER);
    char command_buffer[MAX_INPUT_SIZE + 1] = {0};
    set_config_addr();
    while (1)
    {
        uart_tx_send_str("> ");
        getline_and_send(command_buffer, MAX_INPUT_SIZE);
        char        *arg;
        node_command command = parse_command(command_buffer, &arg);
        do_node_command(command, command_buffer, arg);
    }
}