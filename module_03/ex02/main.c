#include "port.h"
#include "timer.h"

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

#define WAIT_TIME_MS 20

void init_rgb()
{
    timer0_set_prescaler_1();
    timer2_set_prescaler_1();
    timer0_set_mode_phase_correct_pwm_top_ff();
    timer2_set_mode_phase_correct_pwm_top_ff();
    timer0_set_compare_output_mode_phase_correct_pwm_clear_on_upcount_set_on_downcount_oc0a();
    timer0_set_compare_output_mode_phase_correct_pwm_clear_on_upcount_set_on_downcount_oc0b();
    timer2_set_compare_output_mode_phase_correct_pwm_clear_on_upcount_set_on_downcount_oc2b();
}

void set_rgb(uint8_t red, uint8_t green, uint8_t blue)
{
    timer0_set_compare_b(red);
    timer0_set_compare_a(green);
    timer2_set_compare_b(blue);
}

void wheel(uint8_t pos)
{
    pos = 255 - pos;
    if (pos < 85)
    {
        set_rgb(255 - pos * 3, 0, pos * 3);
    }
    else if (pos < 170)
    {
        pos = pos - 85;
        set_rgb(0, pos * 3, 255 - pos * 3);
    }
    else
    {
        pos = pos - 170;
        set_rgb(pos * 3, 255 - pos * 3, 0);
    }
}

void init_gpio()
{
    pd3_output();
    pd5_output();
    pd6_output();
}

int main()
{
    init_gpio();
    init_rgb();
    for (uint8_t i = 0;; i++)
    {
        wheel(i);
        _delay_ms(WAIT_TIME_MS);
    }
}