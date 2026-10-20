#include "adc.h"
#include "interrupt.h"
#include "port.h"
#include "timer.h"

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#define WAIT_TIME_MS 1000
#define BUFFER_SIZE 4
#define TIMER1_FREQ 15625
#define MS 0.064
#define TARGET_WAIT_TIME_MS 20
#define B ((TARGET_WAIT_TIME_MS / MS) + 1)

#define ADC_MAX 255

#define ADC_25_PCT (ADC_MAX / 4)
#define ADC_50_PCT (ADC_MAX / 2)
#define ADC_75_PCT (ADC_MAX * 3 / 4)
#define ADC_100_PCT ADC_MAX
char buffer[BUFFER_SIZE + 1] = {0};

void adc_init()
{
    adc_enable();
    adc_voltage_set_avcc();
    adc_prescaler_set_128();
    adc_enable_interrupt();
    adc_enable_auto_trigger_src_timer1_compare_b();
    adc_read_set_adc0_8_bits();
}

void init_rgb()
{

    pd3_output();
    pd5_output();
    pd6_output();

    timer0_set_prescaler_1();
    timer2_set_prescaler_1();
    timer0_set_mode_fast_pwm_top_ff();
    timer2_set_mode_fast_pwm_top_ff();
    timer0_pwm_enable_a();
    timer0_pwm_enable_b();
    timer2_pwm_enable_b();
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    timer0_set_reg_b(r);
    timer0_set_reg_a(g);
    timer2_set_reg_b(b);
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

void init_led()
{
    pb0_output();
    pb1_output();
    pb2_output();
    pb4_output();
}

ISR(ADC_vect)
{
    uint8_t rv1 = ADCH;
    wheel(rv1);

    PORTB &= ~(0b10111);

    if (rv1 >= ADC_25_PCT)
        pb0_high();

    if (rv1 >= ADC_50_PCT)
        pb1_high();

    if (rv1 >= ADC_75_PCT)
        pb2_high();

    if (rv1 == ADC_100_PCT)
        pb4_high();

    TIFR1 = (1 << OCF1B); // CLEAR OUTPUT COMPARE FLAG B
}

void init_timer1()
{
    timer1_set_compare_b(B);
    timer1_set_compare_a(B);
    timer1_set_mode_ctc();
    timer1_set_prescaler_1024();
}

int main()
{
    adc_init();
    init_timer1();
    init_led();
    init_rgb();
    interrupt_global_enable();

    while (1)
    {
    }
}