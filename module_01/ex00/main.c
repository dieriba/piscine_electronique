#include "port.h"

#include <stdint.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define DELAY_LOOP_TOTAL_CPU_CYCLE ((uint32_t) 4 * (uint16_t) (-1))
#define DELAY_LOOP_TOTAL_MS ((DELAY_LOOP_TOTAL_CPU_CYCLE * 1000) / F_CPU)
#define TARGET_MS 500
#define REQUIRED_ITER_LOOP (TARGET_MS / DELAY_LOOP_TOTAL_MS)

/**

    Delay loop using a 16-bit counter \c __count, so up to 65536
    iterations are possible.  (The value 65536 would have to be
    passed as 0.)  The loop executes four CPU cycles per iteration,
    not including the overhead the compiler requires to setup the
    counter register pair.

    Thus, at a CPU speed of 16 MHz, delays of up to about 16.3
    milliseconds can be achieved.

 */
void delay_loop(uint16_t __count)
{
    __asm__ volatile("1: sbiw %0,1"
                     "\n\t"
                     "brne 1b"
                     : "=w"(__count)
                     : "0"(__count));
}

void wait_half_sec()
{
    for (uint16_t i = 0; i < (uint16_t) REQUIRED_ITER_LOOP; i++)
    {
        delay_loop(-1);
    }
}

void init_gpio()
{
    pb1_output();
}

int main()
{
    init_gpio();
    wait_half_sec();
    while (1)
    {
        pb1_toggle();
        wait_half_sec();
    }
}
