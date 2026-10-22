#include "uart.h"
#include "utils.h"

#include <avr/eeprom.h>
#include <stdint.h>

#define LINE_HEXDUMP_SIZE 77
#define LINE_HEXDUMP_OFFSET_HEX_DATA 9
#define LINE_HEXDUMP_OFFSET_RAW_DATA 57
#define SIZE_EEPROM 0x400
#define NB_LINE_TO_DUMP (SIZE_EEPROM / 0x10)
#define SIZE_LINE 0x10
#define NB_LINE_PER_DUMP 0x10

char dump[LINE_HEXDUMP_SIZE + 1];

void dump_put_line_at(char *dump, uint16_t line)
{
    hex_read_byte_dword(dump, line);
    dump[LINE_HEXDUMP_OFFSET_HEX_DATA - 1] = ' ';
    dump[LINE_HEXDUMP_OFFSET_RAW_DATA]     = '|';
    uint8_t offset_hex_data                = LINE_HEXDUMP_OFFSET_HEX_DATA;
    uint8_t offset_raw_data                = LINE_HEXDUMP_OFFSET_RAW_DATA + 1;
    for (uint8_t i = 0; i < SIZE_LINE; i += 0x4)
    {
        const uint32_t *p    = (const uint32_t *) (line + i);
        uint32_t        data = eeprom_read_dword(p);

        for (uint8_t i = 0; i < sizeof(uint32_t); i++)
        {
            uint8_t b = data & 0xFF;
            hex_read_byte(dump + offset_hex_data, b);
            offset_hex_data += 2; // HEX DIGIT WRITTEN
            dump[offset_hex_data] = ' ';
            offset_hex_data++;
            data >>= 8;

            if (!(b >= 0x20 && b <= 0x7e)) // b is not printable
                b = '.';
            dump[offset_raw_data] = b;
            offset_raw_data++;
        }
    }
    dump[offset_raw_data] = '|';
    offset_raw_data++;
    dump[offset_raw_data] = '\r';
    offset_raw_data++;
    dump[offset_raw_data] = '\n';
}

void hexdump_eeprom()
{
    for (uint8_t i = 0; i < NB_LINE_TO_DUMP; i++)
    {
        dump_put_line_at(dump, i * SIZE_LINE);
        uart_tx_send_str(dump);
    }
}

int main()
{
    uart_init_tx_rx(SENDER);
    eeprom_write_block("dieribasalutcava", (void *) 0, 16);
    eeprom_write_block("dieribasalutcava", (void *) 0x3f0, 16);
    hexdump_eeprom();
    while (1)
    {
    }
}