#include "button.h"
#include "port.h"
#include "timer.h"

#include <stdint.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define PRESCALER 1024UL

#define TOP 15625UL
#define SLOT 10UL
#define VALUE_TO_COMPARE (TOP / 10)

uint16_t A[SLOT];
uint8_t  idx_a;
uint8_t  prev_state_inc_button;
uint8_t  prev_state_dec_button;

/*
    Precompute duty cycle and store it in memory
*/
void init_array_of_a()
{
    for (uint8_t i = 0; i < SLOT; i++)
    {
        float numerator = (i + 1) * 10;
        float a         = ((float) TOP) * (numerator / 100);
        A[i]            = a;
    }
}


/*
    Update OCR1A with next duty cycle up if possible else keep the current one
*/
void inc()
{
    if (idx_a >= (SLOT - 1))
        return;
    idx_a++;
    timer1_set_compare_a(A[idx_a]);
}


/*
    Update OCR1A with previous duty cycle up if possible else keep the current one
*/
void dec()
{
    if (idx_a == 0)
        return;
    idx_a--;
    timer1_set_compare_a(A[idx_a]);
}

void init_timer()
{
    timer1_set_pwm_non_inverting_oc1a();
    timer1_set_mode_fast_pwm_top_icr(TOP);
    timer1_set_prescaler_1024();
    timer1_set_compare_a(A[0]);
}

void init_gpio()
{
    pb1_output();
}

int main()
{
    init_array_of_a();
    init_gpio();
    init_timer();
    while (1)
    {
        button_do_if_new_state(pd2_read, &prev_state_inc_button, inc);
        button_do_if_new_state(pd4_read, &prev_state_dec_button, dec);
    }
}
