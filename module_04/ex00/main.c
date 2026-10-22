#include "interrupt.h"
#include "port.h"

#include <avr/io.h>
#include <util/delay.h>
#define UART_BAUDRATE 115200
#define UBRR_VAL ((F_CPU / (16UL * UART_BAUDRATE)) - 1)
#define DEBOUNCE_TIME 10
uint8_t rising;

SIGNAL(INT0_vect)
{
    _delay_ms(DEBOUNCE_TIME);
    if (rising == 0)
    {
        interrupt_int0_set_rising_edge();
        rising = 1;
        pb0_toggle();
        EIFR = (1 << INTF0);
        return;
    }
    rising = 0;
    interrupt_int0_set_falling_edge();
    EIFR = (1 << INTF0);
}

void init_gpio()
{
    pb0_output();
    pd2_input();
    pd2_low();
    pd2_pull_up_enable();
}

int main(void)
{
    init_gpio();
    interrupt_int0_enable();
    interrupt_int0_set_falling_edge();
    interrupt_global_enable();

    while (1)
    {
    }
}