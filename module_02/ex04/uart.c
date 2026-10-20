
#include "uart.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>

/* 256-byte circular buffers (8-bit indices). */
#define POW 8
#define TX_BUFF_SIZE (1 << POW)
#define RX_BUFF_SIZE (1 << POW)

/* Wrap index around power-of-two buffer size. */
#define pow2_mod(num, pow) ((num) & (pow - 1))

char             tx_buffer[TX_BUFF_SIZE];
volatile uint8_t tx_buffer_head;
volatile uint8_t tx_buffer_tail;

char             rx_buffer[RX_BUFF_SIZE];
volatile uint8_t rx_buffer_head;
volatile uint8_t rx_buffer_tail;

/*
 * Initialize USART0 with baud rate, frame format and TX/RX.
 *
 * UBRR0H/L : 12-bit baud rate divisor.
 * UCSR0C   : UCSZ01:0 = 11 -> 8 data bits, 1 stop bit, no parity.
 * UCSR0B   : TXEN0 = 1 enables TX, RXEN0 = 1 enables RX.
 *            RXCIE0 = 1 enables RX complete interrupt.
 * SREG     : I = 1 enables global interrupts (sei).
 */
void uart_init_tx_rx(unsigned int ubrr, unsigned int tx_rx)
{
    /* Set high and low bytes of baud rate divisor. */
    UBRR0H = (unsigned char) (ubrr >> 8);
    UBRR0L = (unsigned char) ubrr;

    /* UCSZ01:0 = 11: 8-bit data, 1 stop bit, no parity. */
    UCSR0C = (3 << UCSZ00);

    /* Enable transmitter and/or receiver. */
    UCSR0B |= tx_rx;

    /* RXCIE0 = 1: interrupt when a byte is received. */
    if (tx_rx & RECEIVER)
        UCSR0B |= (1 << RXCIE0);

    sei();
}

/*
 * Queue one byte for interrupt-driven transmission.
 *
 * UCSR0B : UDRIE0 = 1 enables data register empty interrupt.
 *          ISR transfers queued bytes into UDR0.
 */
void uart_tx_send_byte(char c)
{
    uint8_t next = pow2_mod(tx_buffer_head + 1, TX_BUFF_SIZE);

    /* Wait until space is available in TX buffer. */
    while (next == tx_buffer_tail)
        ;

    tx_buffer[tx_buffer_head] = c;
    tx_buffer_head            = next;

    /* UDRIE0 = 1: enable USART data register empty interrupt. */
    UCSR0B |= (1 << UDRIE0);
}

/* Queue a null-terminated string for transmission. */
void uart_tx_send_str(char *s)
{
    for (uint16_t i = 0; s[i] != 0; i++)
    {
        uart_tx_send_byte(s[i]);
    }
}

/* Send string followed by CR + LF (\r\n). */
void uart_tx_send_str_with_nl(char *s)
{
    uart_tx_send_str(s);
    uart_tx_send_str("\r\n");
}

/*
 * USART data register empty interrupt.
 *
 * UDR0   : writing loads the next byte for transmission.
 * UCSR0B : UDRIE0 = 0 disables interrupt when TX buffer is empty.
 */
ISR(USART_UDRE_vect)
{
    if (tx_buffer_head != tx_buffer_tail)
    {
        /* Load next byte into USART transmit data register. */
        UDR0           = tx_buffer[tx_buffer_tail];
        tx_buffer_tail = pow2_mod(tx_buffer_tail + 1, TX_BUFF_SIZE);
    }
    else
    {
        /* UDRIE0 = 0: stop interrupts until new data is queued. */
        UCSR0B &= ~(1 << UDRIE0);
    }
}

/*
 * USART receive complete interrupt.
 *
 * UCSR0A : RXC0 = 1 triggers interrupt when data is received.
 * UDR0   : reading retrieves received byte.
 *          Drop byte if software RX buffer is full.
 */
ISR(USART_RX_vect)
{
    char    c    = UDR0;
    uint8_t next = pow2_mod(rx_buffer_head + 1, RX_BUFF_SIZE);

    if (next != rx_buffer_tail)
    {
        rx_buffer[rx_buffer_head] = c;
        rx_buffer_head            = next;
    }
}

/* Return 1 if software RX buffer is empty, otherwise 0. */
uint8_t uart_rx_buffer_is_empty()
{
    return rx_buffer_head == rx_buffer_tail;
}

/* Read next buffered byte; return 0 if buffer is empty. */
uint8_t uart_rx_get_byte()
{
    if (rx_buffer_head == rx_buffer_tail)
        return 0;

    uint8_t c      = rx_buffer[rx_buffer_tail];
    rx_buffer_tail = pow2_mod(rx_buffer_tail + 1, RX_BUFF_SIZE);

    return c;
}

/* Wait until a byte is received, then return it. */
uint8_t uart_rx_blocking_get_byte()
{
    while (uart_rx_buffer_is_empty())
        ;

    return uart_rx_get_byte();
}
