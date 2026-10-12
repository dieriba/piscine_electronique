#ifndef AVR_TIMER_UTILS_H
#define AVR_TIMER_UTILS_H

#include <avr/io.h>
#include <stdint.h>

/*
 * Configure Timer1 prescaler using TCCR1B.
 *
 * CS12:CS10 control the timer clock:
 * 010 = /8, 011 = /64, 100 = /256, 101 = /1024.
 * Clear CS bits before setting the new value.
 */
#define TIMER_GEN_SET_PRESCALER_ROUTINE(TIMER, PRESCALER_VALUE, N)                                                                                                                                     \
    static inline void timer##TIMER##_set_prescaler_##PRESCALER_VALUE(void)                                                                                                                            \
    {                                                                                                                                                                                                  \
        TCCR##TIMER##B = (TCCR##TIMER##B & ~((1 << CS12) | (1 << CS11) | (1 << CS10))) | (N << CS10);                                                                                                  \
    }

/*
 * Enable Timer1 Output Compare A interrupt.
 *
 * TIMSK1: OCIE1A = 1 enables the interrupt when TCNT1 = OCR1A.
 */
#define TIMER_GEN_TIMSK_CONF_ROUTINE(TIMER)                                                                                                                                                            \
    static inline void timer##TIMER##_set_interrupt_on_match_a(void)                                                                                                                                   \
    {                                                                                                                                                                                                  \
        TIMSK##TIMER |= (1 << OCIE1A);                                                                                                                                                                 \
    }

TIMER_GEN_TIMSK_CONF_ROUTINE(1)

/* Generate Timer1 prescaler configuration functions. */
TIMER_GEN_SET_PRESCALER_ROUTINE(1, 8, 2)
TIMER_GEN_SET_PRESCALER_ROUTINE(1, 64, 3)
TIMER_GEN_SET_PRESCALER_ROUTINE(1, 256, 4)
TIMER_GEN_SET_PRESCALER_ROUTINE(1, 1024, 5)

/*
 * Set OCR1A to value.
 * OCR1A defines Compare Match A (or TOP in CTC mode 4).
 */
static inline void timer1_set_compare_a(uint16_t value)
{
    OCR1A = value;
}

/*
 * Set OCR1B to value.
 * OCR1B defines Compare Match B.
 */
static inline void timer1_set_compare_b(uint16_t value)
{
    OCR1B = value;
}

/*
 * Set ICR1 to value.
 * In Fast PWM mode 14, ICR1 defines TOP.
 */
static inline void timer1_set_compare_icr(uint16_t value)
{
    ICR1 = value;
}

/*
 * Configure Timer1 Compare Match A interrupt.
 *
 * OCR1A = value : sets the compare threshold.
 * TIMSK1 OCIE1A = 1 : enables Compare Match A interrupt.
 */
static inline void timer1_interrupt_on_match_a(uint16_t value)
{
    timer1_set_compare_a(value);
    timer1_set_interrupt_on_match_a();
}

/*
 * Select Timer1 Normal mode (WGM13:0 = 0000).
 *
 * TCCR1A: WGM11 = 0, WGM10 = 0.
 * TCCR1B: WGM13 = 0, WGM12 = 0.
 * Timer counts from 0 to 65535.
 */
static inline void timer1_set_default_mode(void)
{
    TCCR1A &= ~((1 << WGM10) | (1 << WGM11));
    TCCR1B &= ~((1 << WGM13) | (1 << WGM12));
}

/*
 * Select Timer1 CTC mode (WGM13:0 = 0100).
 *
 * TCCR1B: WGM12 = 1.
 * Counter resets to 0 after reaching OCR1A (TOP).
 */
static inline void timer1_set_mode_ctc(void)
{
    timer1_set_default_mode();
    TCCR1B |= (1 << WGM12);
}

/*
 * Select Timer1 Fast PWM mode 14 (WGM13:0 = 1110).
 *
 * TCCR1A: WGM11 = 1, WGM10 = 0.
 * TCCR1B: WGM13 = 1, WGM12 = 1.
 * ICR1 defines TOP and the PWM period.
 */
static inline void timer1_set_mode_fast_pwm_top_icr(void)
{
    timer1_set_default_mode();
    TCCR1A |= (1 << WGM11);
    TCCR1B |= (1 << WGM13) | (1 << WGM12);
}

/*
 * Configure OC1A for non-inverting Fast PWM.
 *
 * TCCR1A: COM1A1 = 1, COM1A0 = 0.
 * OC1A is set HIGH at BOTTOM (TCNT1 = 0).
 * OC1A is cleared LOW when TCNT1 = OCR1A.
 *
 * OC1A corresponds to PB1 on ATmega328P.
 * PB1 must be configured as an output (DDRB1 = 1).
 */
static inline void timer1_set_pwm_non_inverting_oc1a(void)
{
    TCCR1A &= ~(1 << COM1A0);
    TCCR1A |= (1 << COM1A1);
}

#endif