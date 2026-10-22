#include "utils.h"
#include <stdint.h>
#define HEX_UPPERCASE "0123456789abcdef"


uint8_t cmp_str(const char *s1, const char *s2)
{
    uint16_t i = 0;

    while (s1[i] && s2[i] && (s1[i] == s2[i]))
        i++;

    return s1[i] - s2[i];
}

uint8_t cmp_mem(const void *mem1, const void *mem2, uint16_t n)
{
    uint16_t    i  = 0;
    const char *m1 = mem1;
    const char *m2 = mem2;

    while (i < n && m1[i] == m2[i])
        i++;

    return m1[i] - m2[i];
}


static uint8_t get_digit_pos_in_hex_base(char digit)
{
    if (digit >= '0' && digit <= '9')
        return digit - '0';
    else if (digit >= 'a' && digit <= 'f')
        return 10 + (digit - 'a');
    else if (digit >= 'A' && digit <= 'F')
        return 10 + (digit - 'A');

    return -1;
}

uint32_t hex_str_to_unsigned(const char *num, const char **end)
{
    uint32_t result = 0;
    
    uint8_t i = 0;
    
    while (num[i])
    {
        uint8_t value = get_digit_pos_in_hex_base(num[i]);
        if (value == (uint8_t)-1)
            break;
        result = (result * 16) + value;
        i++;
    }
    *end = num + i;
    return result;
}

uint32_t dec_str_to_unsigned(const char *num, const char **end)
{
    uint32_t result = 0;
    uint8_t i = 0;
    while (num[i])
    {
        if (!(num[i] >= '0' && num[i] <= '9'))
        {
            *end = num + i;
            break;
        }
        result = (result * 10) + (num[i] - '0');
        i++;
    }
    *end = num + i;
    return result;
}


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

static uint8_t number_count_digits(uint32_t nb)
{
    uint8_t i = (nb == 0);

    while (nb > 0)
    {
        nb /= 10;
        i++;
    }
    return i;
}

static void    number_to_str(char *buffer, uint32_t number, uint8_t neg)
{
    uint8_t nb_digits = number_count_digits(number);


}

void    unsigned_to_str(char *buffer, uint32_t number)
{
    int8_t nb_digits = number_count_digits(number);

    buffer[nb_digits] = 0;
    while (nb_digits >= 0)
    {
        buffer[nb_digits] = (number % 10) + '0';
        nb_digits--;
        number /= 10;
    }
}

