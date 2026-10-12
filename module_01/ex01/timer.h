#ifndef AVR_TIMER_UTILS_H
#define AVR_TIMER_UTILS_H
#include <avr/io.h>

#define TIMER_GEN_SET_PRESCALER_ROUTINE(TIMER, PRESECALER_VALUE, N)                                                                                                                                    \
    static inline void timer##TIMER##_prescaler_##PRESECALER_VALUE()                                                                                                                                   \
    {                                                                                                                                                                                                  \
        TCCR##TIMER##B |= (N << CS10);                                                                                                                                                                 \
    }

#define TIMGER_GEN_TIMSK_CONF_ROUTINE(TIMER)                                                                                                                                                           \
    static inline void timer##TIMER##_set_interrupt_on_ocr1a_match()                                                                                                                                   \
    {                                                                                                                                                                                                  \
        TIMSK##TIMER |= (1 << OCIE1A); /*interrupt on OCR1A match*/                                                                                                                                    \
    }

TIMGER_GEN_TIMSK_CONF_ROUTINE(1)

TIMER_GEN_SET_PRESCALER_ROUTINE(1, 8, 2)
TIMER_GEN_SET_PRESCALER_ROUTINE(1, 64, 3)
TIMER_GEN_SET_PRESCALER_ROUTINE(1, 256, 4)
TIMER_GEN_SET_PRESCALER_ROUTINE(1, 1024, 5)

static inline void timer1_interrupt_on_ocr1a_match(uint16_t value)
{
    timer1_set_interrupt_on_ocr1a_match();
    OCR1A = value;
}

static inline void timer1_ctc(void)
{
    TCCR1A &= ~((1 << WGM10) | (1 << WGM11));
    TCCR1B &= ~((1 << WGM13) | (1 << WGM12));
    TCCR1B |= (1 << WGM12);
}

#endif