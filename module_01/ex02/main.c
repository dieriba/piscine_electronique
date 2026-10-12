#include "port.h"
#include "timer.h"

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define PRESCALER 1024UL

/*
Since the CPU frequency is at 16 MHz, the delay between each cycle is 0.0000625 ms.
The Timer1 counter increments its value at each clock cycle, which means that every 0.0000625 ms there will be an increment.
Our goal is to toggle the LED every 500 ms, so at 16 MHz CPU frequency we'd need to toggle each time the counter reaches 8,000,000,
which is not possible since the counter is 16 bits and has a max value of (2**16 - 1 = 65535).

There are 2 solutions: either count how many times an overflow happens or reduce the frequency so we know a specific counter value means approximately 500 ms have passed.

Prescalers can help us achieve this idea since they allow us to reduce the frequency of the clock by integer division.
There are many values, but we'll choose 1024.

If 16 MHz is the current frequency, with a prescaler of 1024 we'll now have (16,000,000 / 1024) = 15,625 Hz.

*/

#define TOP 15625
#define VALUE_TO_COMPARE (TOP / 10)

void setup(){ 
  pb1_output();
  timer1_set_pwm_non_inverting_oc1a();
  timer1_set_mode_fast_pwm_top_icr();
  timer1_set_compare_icr(TOP);
  timer1_set_prescaler_1024();
  timer1_set_compare_a(VALUE_TO_COMPARE);
} 


int main()
{
    setup();
    while (1)
    {
    }
}
