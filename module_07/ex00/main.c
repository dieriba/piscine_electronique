#include <avr/eeprom.h>
#include <stdint.h>
#include "uart.h"

#define SIZE_LINE_HEXDUMP 76
#define SIZE_EEPROM 0x400
#define NB_LINE_TO_DUMP (SIZE_EEPROM / 0x10)
#define NB_LINE_PER_DUMP 0x10
#define SIZE_DUMP_BUFFER (NB_LINE_PER_DUMP * SIZE_LINE_HEXDUMP)

char dump[SIZE_DUMP_BUFFER + 1];

void set_dump(uint8_t line)
{
    for (uint8_t i = 0; i < NB_LINE_PER_DUMP; i++)
    {
        
    }
}

void hexdump_eeprom()
{
    for (uint8_t i = 0; i < NB_LINE_TO_DUMP; i+= NB_LINE_PER_DUMP)
    {
        set_dump(i);
        uart_tx_send_str(dump);
    }
}

int main()
{
    hexdump_eeprom();
}