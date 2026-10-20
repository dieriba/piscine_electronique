
#include "uart.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#define UART_BAUDRATE 115200UL
#define UBRR_VAL ((F_CPU / (16UL * UART_BAUDRATE)) - 1)

int main()
{
    uart_init_tx_rx(UBRR_VAL + 1, SENDER | RECEIVER);
    while (1)
    {
    }
}