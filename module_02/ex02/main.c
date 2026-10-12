
#include "uart.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

#define UART_BAUDRATE 115200UL
#define UBRR_VAL ((F_CPU / (16UL * UART_BAUDRATE)) - 1)
#define PRESCALER 1024UL
#define TIMER_FREQ (16000000UL / PRESCALER)

void uart_tx(uint8_t byte)
{
    uart_tx_send_byte(byte);
}

int main()
{
    uart_init_tx_rx(UBRR_VAL + 1, SENDER | RECEIVER);
    while (1)
    {
        if (!uart_rx_buffer_is_empty())
        {
            uint8_t byte = uart_rx_get_byte();
            uart_tx(byte);
        }
        _delay_ms(50);
    }
}