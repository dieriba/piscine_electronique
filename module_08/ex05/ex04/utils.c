#include "utils.h"
#include <stdint.h>
#define HEX_UPPERCASE "0123456789abcdef"
#define INT32_MIN_STR "-2147483648"

uint8_t str_cmp(const char *s1, const char *s2)
{
    uint16_t i = 0;

    while (s1[i] && s2[i] && (s1[i] == s2[i]))
        i++;

    return s1[i] - s2[i];
}

uint8_t str_cmp_n(const char *s1, const char *s2, uint16_t size)
{
    uint16_t i = 0;
    size--;
    while (i < size && s1[i] && s2[i] && (s1[i] == s2[i]))
        i++;

    return s1[i] - s2[i];
}

void *mem_copy(void *dst, const void *src, uint16_t size)
{
    char* dest = dst;
    const char* source = src;
    for (uint16_t i = 0; i < size; i++)
    {
        dest[i] = source[i];
    }

    return (void*)(dst + size);
}

void *mem_set(void *dst, char b, uint16_t size)
{
    char* dest = dst;
    for (uint16_t i = 0; i < size; i++)
    {
        dest[i] = b;
    }

    return (void*)(dst + size);
}

uint8_t mem_cmp(const void *mem1, const void *mem2, uint16_t n)
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

int32_t dec_str_to_signed(const char *num, const char **end)
{
    uint32_t result = 0;
    uint8_t i = 0;
    int32_t sign;
    if (num[i] == '-')
    {
        sign = -1;
        i++;
    }
    else
        sign = 1;
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
    return result * sign;
}


static int8_t number_count_digits(uint32_t nb)
{
    uint8_t i = (nb == 0);

    while (nb > 0)
    {
        nb /= 10;
        i++;
    }
    return i;
}


uint8_t    signed_to_str(int32_t number, char *buffer)
{
    if (number == INT32_MIN)
    {
        mem_copy(buffer, INT32_MIN_STR, sizeof(INT32_MIN_STR));
        return sizeof(INT32_MIN_STR) - 1;
    }


    int8_t sign = number < 0 ? -1 : 1;
    number *= sign;
    int8_t nb_digits = number_count_digits(number);

    uint8_t stop;
    if (sign < 0)
    {
        buffer[0] = '-';
        stop = 1;
        nb_digits++;
    }
    else
        stop = 0;


    buffer[nb_digits] = 0;
    int8_t i = nb_digits - 1;
    while (i >= stop)
    {
        buffer[i] = (number % 10) + '0';
        number /= 10;
        i--;
    }

    return nb_digits;
}

uint8_t unsigned_to_str(uint32_t number, char *buffer)
{
    int8_t nb_digits = number_count_digits(number);

    buffer[nb_digits] = 0;
    int8_t i = nb_digits - 1;
    while (i >= 0)
    {
        buffer[i] = (number % 10) + '0';
        number /= 10;
        i--;
    }

    return nb_digits;
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

