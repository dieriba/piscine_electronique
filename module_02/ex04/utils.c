#include "utils.h"

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
