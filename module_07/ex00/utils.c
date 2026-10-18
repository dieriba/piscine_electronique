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

void hex_read_dword(char *buffer, uint32_t hex)
{
    for (int8_t i = sizeof(uint32_t) - 1; i >= 0; i--)
    {
        hex_read(buffer + (i * 2), hex & 0xFF);
        hex >>= 8;
    }
}


static uint8_t number_count_digit(int32_t number)
{
    uint8_t count = number <= 0;
    while (number != 0)
    {
        number /= 10;
        count++;
    }
    return count;
}

uint8_t itoa(char *buffer, int32_t number)
{
    uint8_t number_nb_digit = number_count_digit(number);

    int8_t len = number_nb_digit;
    int8_t stop;
    if (number < 0)
    {
        buffer[0] = '-';
        stop      = 1;
        number *= -1; //FIX IT OVERFLOW BUG MAY HAPPEN
    }
    else
        stop = 0;

    buffer[len] = 0;
    len--;
    while (len >= stop)
    {
        buffer[len] = (number % 10) + '0';
        number /= 10;
        len--;
    }

    return number_nb_digit;
}