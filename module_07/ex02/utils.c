#include "utils.h"

#include <stdint.h>

#define HEX_UPPERCASE "0123456789abcdef"

void hex_read_byte(char *buffer, uint8_t hex)
{
    char low_nibble  = HEX_UPPERCASE[hex & 0x0f];
    char high_nibble = HEX_UPPERCASE[hex >> 4];

    buffer[0] = high_nibble;
    buffer[1] = low_nibble;
}

void hex_read_byte_dword(char *buffer, uint32_t hex)
{
    for (int8_t i = sizeof(uint32_t) - 1; i >= 0; i--)
    {
        hex_read_byte(buffer + (i * 2), hex & 0xFF);
        hex >>= 8;
    }
}
