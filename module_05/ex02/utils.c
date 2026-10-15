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

static uint8_t number_count_digit(uint16_t number)
{
    uint8_t count = number == 0;
    while (number != 0)
    {
        number /= 10;
        count++;
    }
    return count;
}

uint8_t itoa(char *buffer, uint16_t number)
{
    uint8_t number_nb_digit = number_count_digit(number);

    int8_t len  = number_nb_digit;
    buffer[len] = 0;
    len--;
    while (len >= 0)
    {
        buffer[len] = (number % 10) + '0';
        number /= 10;
        len--;
    }

    return number_nb_digit;
}