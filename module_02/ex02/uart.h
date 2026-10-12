#ifndef UART_H
#define UART_H

#include <avr/io.h>
#include <stddef.h>
#include <stdint.h>

#define SENDER (1 << TXEN0)
#define RECEIVER (1 << RXEN0)

void    uart_init_tx_rx(unsigned int ubrr, unsigned int tx_rx);
void    uart_flush(void);
void    uart_block_until_send_buffer_is_empty();
void    uart_tx_send_byte(uint8_t c);
void    uart_tx_send_str(uint8_t *s);
uint8_t uart_rx_get_byte();
uint8_t uart_rx_buffer_is_empty();
#endif