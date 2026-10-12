#ifndef UART_H
#define UART_H

#include <avr/io.h>
#include <stddef.h>

#define SENDER (1 << TXEN0)
#define RECEIVER (1 << RXEN0)

void uart_init_tx_rx(unsigned int ubrr, unsigned int tx_rx);
void uart_flush(void);
void uart_block_until_send_buffer_is_empty();
void uart_send_byte(unsigned char c);
void uart_send_str(unsigned char *s);

#endif