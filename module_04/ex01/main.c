#include "interrupt.h"
#include "port.h"
#include "timer.h"

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

#define NB_DUTY_CYCLE 20
#define TIMER1_PRESCALER 1
#define TIMER1_PWN_DESIRED_FREQ 1000
#define COMPUTE_TOP_PWN_FROM_DESIRED_FREQ(TIMER_FREQ, TIMER_PRESCALER, DESIRED_FREQ) ((TIMER_FREQ / (TIMER_PRESCALER * DESIRED_FREQ)) - 1)
#define TIMER1_TOP (uint16_t) COMPUTE_TOP_PWN_FROM_DESIRED_FREQ(F_CPU, TIMER1_PRESCALER, TIMER1_PWN_DESIRED_FREQ)

#define TIMER_F_CPU 15625
#define TIMER_0_TOP 0x100
#define TIMER_0_NB_OVERFLOW_1_SEC (TIMER_F_CPU / TIMER_0_TOP)
#define TIMER_0_NB_OVERFLOW_50_MS (TIMER_0_NB_OVERFLOW_1_SEC / 2 / 10)

uint16_t duty_cycles[NB_DUTY_CYCLE];
uint8_t  timer0_ovf_count;
uint8_t  idx;

SIGNAL(TIMER0_OVF_vect)
{
    if (timer0_ovf_count == (TIMER_0_NB_OVERFLOW_50_MS - 1))
    {
        timer1_set_compare_a(duty_cycles[idx]);
        timer0_ovf_count = 0;
        idx++;
        if (idx == NB_DUTY_CYCLE)
            idx = 0;
        return;
    }
    timer0_ovf_count++;
}

void init_gpio()
{
    pb1_output();
}

void init_timer()
{
    timer1_set_compare_output_mode_fast_pwm_set_at_bottom_clear_on_match_oc1a();
    uint16_t top = COMPUTE_TOP_PWN_FROM_DESIRED_FREQ(F_CPU, TIMER1_PRESCALER, TIMER1_PWN_DESIRED_FREQ);
    timer1_set_mode_fast_pwm_top_icr(top);
    timer1_set_compare_a(0);
    timer1_set_prescaler_1();

    timer0_set_mode_normal();
    timer0_set_interrupt_on_overflow();
    timer0_set_prescaler_1024();
    interrupt_global_enable();
}

void init_duty_cycle()
{
    uint8_t i       = 1;
    float   percent = 0.1;
    float   top     = TIMER1_TOP;
    for (; i <= (NB_DUTY_CYCLE / 2); i++)
    {
        duty_cycles[i] = top * percent;
        percent += 0.1;
    }
    uint8_t j = i - 2;
    for (; i < NB_DUTY_CYCLE; i++)
    {
        duty_cycles[i] = duty_cycles[j];
        j--;
    }
}

int main(void)
{
    init_duty_cycle();
    init_gpio();
    init_timer();

    while (1)
        ;
}
