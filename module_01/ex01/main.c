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
Now the delay between each cycle is 0.064 ms, so the counter needs to count:

500 / 0.064 = 7812.5 ticks.

Since we're using CTC mode, the counter counts from 0 to OCR1A (OCR1A + 1 ticks).
We'll choose OCR1A = 7812, which gives us:

(7812 + 1) * 0.064 = 500.032 ms.

Each time Timer1 reaches 7812, approximately 500 ms have passed, and we can toggle the LED.
*/

#define VALUE_TO_COMPARE 7812

void __attribute__((signal, used)) __vector_11(void)
{
    pb1_toggle();
}

void enable_interrupt()
{
    __asm__ __volatile__("sei" ::: "memory");
}

void init_gpio()
{
    pb1_output();
    timer1_ctc();
    timer1_interrupt_on_ocr1a_match(VALUE_TO_COMPARE);
    timer1_prescaler_1024();
    enable_interrupt();
}

int main()
{
    init_gpio();
    while (1)
    {
    }
}
