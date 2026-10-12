
#include "uart.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

#define UART_BAUDRATE 9600
#define UBRR_VAL ((F_CPU / (16UL * UART_BAUDRATE)) - 1)

void uart_tx(uint8_t byte)
{
    uart_tx_send_byte(byte);
}

char uart_rx()
{
    while (uart_rx_buffer_is_empty())
        ;
    uint8_t byte = uart_rx_get_byte();
    return byte;
}

int main()
{
    uart_init_tx_rx(UBRR_VAL + 1, SENDER | RECEIVER);
    while (1)
    {
        uint8_t byte = uart_rx();
        uart_tx(byte);
    }
}