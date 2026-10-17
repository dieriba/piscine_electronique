#include "utils.h"

#include <stdint.h>

#define HEX_UPPERCASE "0123456789abcdef"

void hex_read(char *buffer, uint8_t hex)
{
    char low_nibble  = HEX_UPPERCASE[hex & 0x0f];
    char high_nibble = HEX_UPPERCASE[hex >> 4];

    buffer[0] = high_nibble;
    buffer[1] = low_nibble;
}
