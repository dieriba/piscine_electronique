#include "interrupt.h"
#include "port.h"

#include <avr/io.h>

SIGNAL(INT0_vect)
{
    pb0_toggle();
}

void init_gpio()
{
    pb0_output();
    pd2_input();
    pd2_pull_up_enable();
}

int main(void)
{
    init_gpio();
    interrupt_int0_set_falling_edge();
    interrupt_int0_enable();
    interrupt_global_enable();

    while (1)
    {
    }
}