#ifndef UART_H
#define UART_H

#include <avr/io.h>
#include <stddef.h>
#include <stdint.h>

#define SENDER (1 << TXEN0)
#define RECEIVER (1 << RXEN0)

void    uart_init_tx_rx(unsigned int ubrr, unsigned int tx_rx);
void    uart_block_until_send_buffer_is_empty();
void    uart_tx_send_byte(char c);
void    uart_tx_send_str(char *s);
void    uart_tx_send_str_with_nl(char *s);
uint8_t uart_rx_get_byte();
uint8_t uart_rx_blocking_get_byte();
uint8_t uart_rx_buffer_is_empty();
#endif