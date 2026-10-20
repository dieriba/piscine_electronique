
#include "timer.h"
#include "uart.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#define UART_BAUDRATE 115200UL
#define UBRR_VAL ((F_CPU / (16UL * UART_BAUDRATE)) - 1)
#define MSG "Hello World!"
#define PRESCALER 1024UL
#define TIMER_FREQ (16000000UL / PRESCALER)
#define VALUE_TO_COMPARE (TIMER_FREQ * 2)

void uart_printstr()
{
    uart_tx_send_str_with_nl(MSG);
}

SIGNAL(TIMER1_COMPA_vect)
{
    uart_printstr();
}

void init_timer()
{
    timer1_set_mode_ctc();
    timer1_set_prescaler_1024();
    timer1_interrupt_on_match_a(VALUE_TO_COMPARE);
    __asm__ __volatile__("sei" ::: "memory"); /*ENABLE GLOBAL INTERRUPT*/
}

int main()
{
    uart_init_tx_rx(UBRR_VAL + 1, SENDER);
    init_timer();
    while (1)
    {
    }
}