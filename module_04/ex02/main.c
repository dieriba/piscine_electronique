#include "interrupt.h"
#include "port.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>

#define pow2(pow) (1 << (pow))
#define pow2_mod(numerator, modulus) ((numerator) & ((modulus) - 1))
#define POW pow2(4)
#define MASK_PB4_STATE (0b1000)

uint8_t value;
uint8_t mask_clear_prev_val;
uint8_t prev_state_dec_button;

void init_gpio()
{
    pd2_pull_up_enable();
    pd4_pull_up_enable();
    prev_state_dec_button = pd4_read();
    pb0_output();
    pb1_output();
    pb2_output();
    pb4_output();
}

void repr_value_from_led_d1_d2_d3_d4(uint8_t value)
{
    uint8_t real_value = pow2_mod(value, POW);
    uint8_t pb4_set    = (real_value & MASK_PB4_STATE) << 1; // PB3 MOVE AS PB4
    real_value &= 0b0111;
    PORTB ^= mask_clear_prev_val;
    PORTB |= pb4_set | real_value;
    mask_clear_prev_val = pb4_set | real_value;
}

void init_interrupt()
{
    interrupt_int0_set_falling_edge();
    interrupt_int0_enable();

    interrupt_pcint_group2_enable();
    interrupt_pd4_enable();
    interrupt_global_enable();
}

ISR(TIMER1_COMPA_vect)
{
    pb0_toggle();
}

ISR(INT0_vect)
{
    repr_value_from_led_d1_d2_d3_d4(++value);
}

ISR(PCINT2_vect)
{
    uint8_t current_state = pd4_read();

    if (current_state != prev_state_dec_button)
    {
        prev_state_dec_button = current_state;
        if (current_state == 0)
        {
            repr_value_from_led_d1_d2_d3_d4(--value);
        }
    }
}

int main()
{
    init_gpio();
    init_interrupt();
    while (1)
    {
    }
}