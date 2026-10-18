#include "uart.h"

#include <avr/eeprom.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <util/crc16.h>
#include <util/delay.h>

#define BACKSPACE 0x7f
#define ERASE_CHAR_TERM "\b \b"
#define MAX_INPUT_SIZE 0x100

#define NB_CONFIG 4
#define CONFIG_TAG_SIZE 32

#define NODE_COMMAND_STATUS_STR "STATUS"
#define NODE_COMMAND_SET_ID_STR "SET_ID"
#define NODE_COMMAND_SET_PRIORITY_STR "SET_PRIO"
#define NODE_COMMAND_SET_TAG_STR "SET_TAG"
#define NODE_COMMAND_SET_SLOT_STR "SET_SLOT"
#define NODE_COMMAND_FACTORY_RESET_STR "FACTORY_RESET"
#define LEN_LITERAL_STR(s) (sizeof(s) - 1)
#define MAGIC_NUMBER 0xDEEDBEEF

#define BASE_ADDR_NODE_CONFIG 0

#define FIELD_SIZE(type, field) sizeof(((type *) 0)->field)
#define LEN_NUMBER 11
char NUM_BUFFER[LEN_NUMBER + 1];

typedef struct node_config
{
    char     tag[CONFIG_TAG_SIZE + 1];
    uint32_t magic_number;
    uint16_t priority;
    uint16_t crc;
    uint8_t  id;
} node_config;

#define ADDR_NODE_CONFIG(i) (node_config *) (i * sizeof(node_config))

uint16_t addr_config;
#define GET_CURRENT_NODE_CONFIG (node_config *) (addr_config)

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
    if (strncmp(command, command_to_match, len_command_to_match) == 0 && command[len_command_to_match] == ' ')
    {
        *arg = command + len_command_to_match + 1;
        return 1;
    }
    return 0;
}

node_command parse_command(char *command, char **arg)
{
    if (strcmp(command, NODE_COMMAND_STATUS_STR) == 0)
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

    if (check_command_with_args(command, NODE_COMMAND_FACTORY_RESET_STR, LEN_LITERAL_STR(NODE_COMMAND_FACTORY_RESET_STR), arg))
        return NODE_COMMAND_FACTORY_RESET;

    return NODE_COMMAND_UNKNOWN;
}

uint8_t getline_and_send(char *buffer, uint16_t max_len)
{
    uint8_t i = 0;
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

uint8_t is_eeprom_byte_at_eq(uint16_t addr, uint8_t cmp)
{
    return eeprom_read_byte((const uint8_t *) addr) == cmp;
}

uint8_t update_config_eeprom(uint8_t offset_field, uint8_t *data, uint16_t len)
{
    uint16_t addr_field     = addr_config + offset_field;
    uint16_t end_addr_field = addr_field + len;
    for (; addr_field < end_addr_field; addr_field++)
    {
        uint8_t b = *data;
        eeprom_update_byte((uint8_t *) addr_field, b);
        if (!is_eeprom_byte_at_eq(addr_field, b))
            return 0;
        data++;
    }

    return 1;
}

void node_config_print(node_config *config)
{
    uart_tx_send_str("Node ID: ");
    uint32_t id = eeprom_read_dword((const uint32_t *) &config->id);
    utoa(id, NUM_BUFFER, 10);
    uart_tx_send_str_with_nl(NUM_BUFFER);

    uart_tx_send_str("Priority: ");
    uint16_t priority = eeprom_read_word((const uint16_t *) &config->priority);
    utoa(priority, NUM_BUFFER, 10);
    uart_tx_send_str_with_nl(NUM_BUFFER);

    uart_tx_send_str("Slot: ");
    utoa(addr_config / sizeof(node_config), NUM_BUFFER, 10);
    uart_tx_send_str_with_nl(NUM_BUFFER);

    uart_tx_send_str("Tag: ");
    // add read
}

void do_status()
{
    node_config *config = GET_CURRENT_NODE_CONFIG;

    // do crc check

    node_config_print(config);
}

void do_set_id(const char *arg)
{
}

void do_set_priority(const char *arg)
{
}

void do_set_tag(const char *arg)
{
}

void do_factory_reset()
{

    eeprom_update_byte((unsigned char *) GET_CURRENT_NODE_CONFIG, 0);
    if (eeprom_read_byte((const uint8_t *) GET_CURRENT_NODE_CONFIG) != 0)
    {
        //CRITICAL ERROR
    }
}

void do_node_command(node_command command, const char *cmd, const char *arg)
{
    switch (command)
    {
    case NODE_COMMAND_STATUS:
        do_status();
        break;

    case NODE_COMMAND_SET_ID:
        do_set_id(arg);
        break;

    case NODE_COMMAND_SET_PRIORITY:
        do_set_priority(arg);
        break;

    case NODE_COMMAND_SET_TAG:
        do_set_tag(arg);
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
    addr_config = BASE_ADDR_NODE_CONFIG;
    for (uint8_t i = 0; i < NB_CONFIG; i++)
    {
        node_config *config = ADDR_NODE_CONFIG(i);
        if (config->magic_number == MAGIC_NUMBER)
        {
            addr_config = i;
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