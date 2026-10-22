#include "uart.h"
#include "utils.h"

#include <avr/eeprom.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>

#define LINE_HEXDUMP_SIZE 0x100
#define LINE_HEXDUMP_OFFSET_HEX_DATA 9
#define SIZE_EEPROM 0x400
#define NB_LINE_TO_DUMP (SIZE_EEPROM / 0x10)
#define SIZE_LINE 0x10
#define NB_LINE_PER_DUMP 0x10
#define BACKSPACE 0x7f
#define ERASE_CHAR_TERM "\b \b"
#define SIZE_INPUT 6
#define MAX_VALUE_ADDR 0x3ff
#define TERM_SET_RED_COLOR "\x1b[31m"
#define TERM_RESET_COLOR "\x1b[0m"

char dump[LINE_HEXDUMP_SIZE + sizeof(TERM_RESET_COLOR) + sizeof(TERM_RESET_COLOR) + 1];

void dump_put_line_at(char *dump, uint16_t cursor, uint16_t pos_byte_to_highlight)
{
    hex_read_byte_dword(dump, cursor);
    dump[LINE_HEXDUMP_OFFSET_HEX_DATA - 1] = ' ';
    uint8_t  offset_hex_data               = LINE_HEXDUMP_OFFSET_HEX_DATA;
    uint16_t end_cursor                    = cursor + SIZE_LINE;
    while (cursor < end_cursor)
    {
        const uint32_t *p    = (const uint32_t *) cursor;
        uint32_t        data = eeprom_read_dword(p);
        uint16_t        end  = cursor + sizeof(uint32_t);
        for (; cursor < end; cursor++)
        {
            if (end - cursor == 2)
            {
                dump[offset_hex_data] = ' ';
                offset_hex_data++;
            }
            uint8_t b = data & 0xFF;
            if (cursor == pos_byte_to_highlight)
            {
                memcpy(dump + offset_hex_data, TERM_SET_RED_COLOR, sizeof(TERM_SET_RED_COLOR) - 1);
                offset_hex_data += sizeof(TERM_SET_RED_COLOR) - 1;
                hex_read_byte(dump + offset_hex_data, b);
                offset_hex_data += 2; // HEX DIGIT WRITTEN
                memcpy(dump + offset_hex_data, TERM_RESET_COLOR, sizeof(TERM_RESET_COLOR) - 1);
                offset_hex_data += sizeof(TERM_RESET_COLOR) - 1;
            }
            else
            {
                hex_read_byte(dump + offset_hex_data, b);
                offset_hex_data += 2; // HEX DIGIT WRITTEN
            }
            data >>= 8;
        }
        dump[offset_hex_data] = ' ';
        offset_hex_data++;
    }
    dump[offset_hex_data - 1] = '\r';
    dump[offset_hex_data]     = '\n';
    offset_hex_data++;
    dump[offset_hex_data] = 0;
}

void hexdump_eeprom(uint16_t pos_byte_to_highlight)
{

    for (uint8_t i = 0; i < NB_LINE_TO_DUMP; i++)
    {
        dump_put_line_at(dump, i * SIZE_LINE, pos_byte_to_highlight);
        uart_tx_send_str(dump);
    }
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

uint8_t is_hex_digit(char c)
{
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'Z');
}

uint8_t parse_input(char *input, uint16_t *pos_byte, uint8_t *new_byte_value)
{
    char    *end;
    uint32_t pos = strtol(input, &end, 16);

    uint8_t dist = end - input;
    if (pos > MAX_VALUE_ADDR || *end != ' ' || (dist != 2 && dist != 3))
        return 0;

    input          = end + 1;
    uint32_t new_b = strtol(input, &end, 16);

    if (*end != '\0' || (end - input) != 2)
        return 0;

    *pos_byte       = pos;
    *new_byte_value = new_b;

    return 1;
}

int main()
{
    uart_init_tx_rx(SENDER | RECEIVER);
    eeprom_write_block("dieribasalutcava", (void *) 0, 16);
    char input_buffer[SIZE_INPUT + 1] = {0};
    while (1)
    {
        uart_tx_send_str("> ");
        getline_and_send(input_buffer, SIZE_INPUT);
        uint16_t pos_byte_to_highlight;
        uint8_t  new_byte_value;
        parse_input(input_buffer, &pos_byte_to_highlight, &new_byte_value);
        if (eeprom_read_byte((const uint8_t *) pos_byte_to_highlight) != new_byte_value)
        {
            eeprom_write_byte((unsigned char *) pos_byte_to_highlight, new_byte_value);
            hexdump_eeprom(pos_byte_to_highlight);
        }
    }
}