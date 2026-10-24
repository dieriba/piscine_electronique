#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
uint8_t mem_cmp(const void *mem1, const void *mem2, uint16_t n);
void *mem_copy(void *dst, const void *src, uint16_t size);
void *mem_set(void *src, char b, uint16_t size);
uint8_t str_cmp(const char *s1, const char *s2);
uint8_t str_cmp_n(const char *s1, const char *s2, uint16_t size);
uint32_t hex_str_to_unsigned(const char *num, const char **end);
uint32_t dec_str_to_unsigned(const char *num, const char **end);
int32_t dec_str_to_signed(const char *num, const char **end);
uint8_t    signed_to_str(int32_t number, char *buffer);
uint8_t unsigned_to_str(uint32_t number, char *buffer);
void hex_read_byte(char *buffer, uint8_t hex);
void    hex_read_byte_dword(char *buffer, uint32_t hex);
#endif