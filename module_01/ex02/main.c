#include "port.h"
#include "timer.h"

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define PRESCALER 1024UL
#define TOP 15625
#define VALUE_TO_COMPARE (TOP / 10)

void init_timer(){ 
  timer1_set_pwm_non_inverting_oc1a();
  timer1_set_mode_fast_pwm_top_icr(TOP);
  timer1_set_prescaler_1024();
  timer1_set_compare_a(VALUE_TO_COMPARE);
} 


void  init_gpio()
{
  pb1_output();
}

int main()
{
    init_gpio();
    init_timer();
    while (1)
    {
    }
}
