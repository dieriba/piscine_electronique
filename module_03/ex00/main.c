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
    timer0_set_mode_fast_pwm_top_ff();
    timer2_set_mode_fast_pwm_top_ff();
    timer0_pwm_enable_a();
    timer0_pwm_enable_b();
    timer2_pwm_enable_b();
}

void init_gpio()
{
    pd3_output();
    pd5_output();
    pd6_output();
}

void set_rgb(uint8_t red, uint8_t green, uint8_t blue)
{
    timer0_set_reg_b(red);
    timer0_set_reg_a(green);
    timer2_set_reg_b(blue);
}

int main()
{
    init_gpio();
    init_rgb();
    uint8_t i = 0;
    while (1)
    {
        i = i % RGB_CHANNELS;
        set_rgb(rgb_channels[i].red, rgb_channels[i].green, rgb_channels[i].blue);
        i++;
        _delay_ms(WAIT_TIME_MS);
    }
}