#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
uint8_t cmp_str(const char *s1, const char *s2);
uint8_t cmp_mem(const void *mem1, const void *mem2, uint16_t n);

#endif