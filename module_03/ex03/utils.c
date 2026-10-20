#include "utils.h"
#include <stdint.h>

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
        return digit - 'a';
    else if (digit >= 'A' && digit <= 'F')
        return digit - 'A';

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
