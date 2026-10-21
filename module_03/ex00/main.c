#include "port.h"
#include "timer.h"

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

#define WAIT_TIME_MS 1000

typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} rgb_color;

static const rgb_color rgb_channels[] = {
    {0xFF, 0x00, 0x00}, // Red
    {0x00, 0xFF, 0x00}, // Green
    {0x00, 0x00, 0xFF}, // Blue
};

#define RGB_CHANNELS (sizeof(rgb_channels) / sizeof(rgb_channels[0]))

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
    uint8_t i = 0;
    while (1)
    {
        set_rgb(rgb_channels[i].red, rgb_channels[i].green, rgb_channels[i].blue);
        _delay_ms(WAIT_TIME_MS);
        i = (i + 1) % RGB_CHANNELS;
    }
}