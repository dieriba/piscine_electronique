#include "uart.h"

#include <util/delay.h>

#define UART_BAUDRATE 115200UL
#define UBRR_VAL ((F_CPU / (16UL * UART_BAUDRATE)) - 1)
#define WAIT_TIME_MS 1000

void uart_tx()
{
    uart_send_byte('Z');
}

int main()
{
    uart_init_tx_rx(UBRR_VAL + 1, SENDER);
    while (1)
    {
        uart_tx();
        _delay_ms(WAIT_TIME_MS);
    }
}