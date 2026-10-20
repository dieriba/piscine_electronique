#include "uart.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#define POW 8
#define TX_BUFF_SIZE (1 << POW)
#define RX_BUFF_SIZE (1 << POW)
#define UART_BAUDRATE 9600
#define UBRR_VAL ((F_CPU / (16UL * UART_BAUDRATE)) - 1)

#define pow2_mod(num, pow) ((num) & (pow - 1))

char             tx_buffer[TX_BUFF_SIZE];
volatile uint8_t tx_buffer_head;
volatile uint8_t tx_buffer_tail;

char             rx_buffer[TX_BUFF_SIZE];
volatile uint8_t rx_buffer_head;
volatile uint8_t rx_buffer_tail;

void uart_init_tx_rx(unsigned int tx_rx)
{
    /*Set baud rate */
    unsigned int ubrr = UBRR_VAL;
    UBRR0H            = (unsigned char) (ubrr >> 8);
    UBRR0L            = (unsigned char) ubrr;

    /* Set frame format: 8 data, 1 stop bit */
    UCSR0C = (3 << UCSZ00);
    /*Enable transmitter and/or receiver */
    UCSR0B |= tx_rx;
    if (tx_rx & RECEIVER)
        UCSR0B |= (1 << RXCIE0); // Enable Rx Interrupt
    __asm__ __volatile__("sei" ::: "memory");
}

void uart_flush(void)
{
    volatile unsigned char dummy = 0;
    (void) dummy;
    while (UCSR0A & (1 << RXC0))
        dummy = UDR0;
}

void uart_tx_send_byte(char c)
{
    uint8_t next = pow2_mod(tx_buffer_head + 1, TX_BUFF_SIZE);
    while (next == tx_buffer_tail)
        ; /* Wait if buffer full */
    tx_buffer[tx_buffer_head] = c;
    tx_buffer_head            = next;
    UCSR0B |= (1 << UDRIE0); /* Enable UDRE interrupt */
}

void uart_tx_send_str(char *s)
{
    for (uint16_t i = 0; s[i] != 0; i++)
    {
        uart_tx_send_byte(s[i]);
    }
}

SIGNAL(USART_UDRE_vect)
{
    if (tx_buffer_head != tx_buffer_tail)
    {
        UDR0           = tx_buffer[tx_buffer_tail];
        tx_buffer_tail = pow2_mod(tx_buffer_tail + 1, TX_BUFF_SIZE);
    }
    else
    {
        UCSR0B &= ~(1 << UDRIE0); /* Buffer empty, disable interrupt */
    }
}

SIGNAL(USART_RX_vect)
{
    char    c    = UDR0;
    uint8_t next = pow2_mod(rx_buffer_head + 1, RX_BUFF_SIZE);
    if (next != rx_buffer_tail)
    {
        rx_buffer[rx_buffer_head] = c;
        rx_buffer_head            = next;
    }
}

uint8_t uart_rx_buffer_is_empty()
{
    return rx_buffer_head == rx_buffer_tail;
}

uint8_t uart_rx_get_byte()
{
    if (rx_buffer_head == rx_buffer_tail)
        return 0;
    uint8_t c      = rx_buffer[rx_buffer_tail];
    rx_buffer_tail = pow2_mod(rx_buffer_tail + 1, RX_BUFF_SIZE);
    return c;
}

uint8_t uart_rx_blocking_get_byte()
{
    while (uart_rx_buffer_is_empty())
        ;
    return uart_rx_get_byte();
}