#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
uint8_t cmp_str(const char *s1, const char *s2);
uint8_t cmp_mem(const void *mem1, const void *mem2, uint16_t n);
uint32_t hex_str_to_unsigned(const char *num, const char **end);
uint32_t dec_str_to_unsigned(const char *num, const char **end);
void hex_read_byte(char *buffer, uint8_t hex);
void    hex_read_byte_dword(char *buffer, uint32_t hex);
void mem_copy(void *dst, const void* src, uint16_t size);
uint8_t    unsigned_number_to_str(char *buffer, uint32_t number);
uint8_t    signed_number_to_str(char *buffer, int32_t number);
uint8_t itoa(char *buffer, int32_t number);
#endif