#include "button.h"
#include "port.h"

#include <avr/io.h>
#include <util/delay.h>

#define pow2(pow) (1 << (pow))
#define pow2_mod(numerator, modulus) ((numerator) & ((modulus) - 1))
#define POW pow2(4)
#define MASK_PB4_STATE (0b1000)
#define LOCK_TIME 100

uint8_t value;
uint8_t mask_clear_prev_val;
uint8_t prev_state_inc_button;
uint8_t prev_state_dec_button;

void init_gpio()
{
    pd2_pull_up_enable();
    prev_state_inc_button = pd2_read();
    pd4_pull_up_disable();
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

void inc()
{
    repr_value_from_led_d1_d2_d3_d4(++value);
}

void dec()
{
    repr_value_from_led_d1_d2_d3_d4(--value);
}

int main()
{
    init_gpio();
    while (1)
    {
        button_do_if_new_state(pd2_read, &prev_state_inc_button, inc);
        button_do_if_new_state(pd4_read, &prev_state_dec_button, dec);
        _delay_ms(LOCK_TIME);
    }
}