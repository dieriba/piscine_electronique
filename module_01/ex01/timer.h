
#ifndef AVR_TIMER_UTILS_H
#define AVR_TIMER_UTILS_H

#include <avr/io.h>
#include <stdint.h>


/*
 * Generate Timer1 prescaler functions.
 *
 * TCCR1B CS12:10:
 * 010 = /8, 011 = /64, 100 = /256, 101 = /1024.
 */
#define TIMER_GEN_SET_PRESCALER_ROUTINE(TIMER, PRESCALER, N) \
    /* Set CS12:10 = N: divide timer clock by PRESCALER. */ \
    static inline void timer##TIMER##_set_prescaler_##PRESCALER(void) \
    { \
        TCCR##TIMER##B = (TCCR##TIMER##B & \
            ~((1 << CS12) | (1 << CS11) | (1 << CS10))) | \
            ((N) << CS10); \
    }

/* Generate Timer1 interrupt enable function. */
#define TIMER_GEN_TIMSK_CONF_ROUTINE(TIMER) \
    /* TIMSK1 OCIE1A = 1: enable Compare Match A interrupt. */ \
    static inline void timer##TIMER##_set_interrupt_on_match_a(void) \
    { \
        TIMSK##TIMER |= (1 << OCIE1A); \
    }

TIMER_GEN_TIMSK_CONF_ROUTINE(1)

TIMER_GEN_SET_PRESCALER_ROUTINE(1, 8, 2)
TIMER_GEN_SET_PRESCALER_ROUTINE(1, 64, 3)
TIMER_GEN_SET_PRESCALER_ROUTINE(1, 256, 4)
TIMER_GEN_SET_PRESCALER_ROUTINE(1, 1024, 5)

#undef TIMER_GEN_SET_PRESCALER_ROUTINE
#undef TIMER_GEN_TIMSK_CONF_ROUTINE

/* OCR1A = value: set Compare Match A threshold (TOP in CTC mode 4). */
static inline void timer1_set_compare_a(uint16_t value)
{
    OCR1A = value;
}

/* OCR1B = value: set Compare Match B threshold. */
static inline void timer1_set_compare_b(uint16_t value)
{
    OCR1B = value;
}

/* ICR1 = value: set TOP and PWM period in Fast PWM mode 14. */
static inline void timer1_set_top_icr(uint16_t value)
{
    ICR1 = value;
}

/*
 * OCR1A = value: set Compare Match A threshold.
 * TIMSK1 OCIE1A = 1: enable Compare Match A interrupt.
 */
static inline void timer1_interrupt_on_match_a(uint16_t value)
{
    timer1_set_compare_a(value);
    timer1_set_interrupt_on_match_a();
}

/*
 * TCCR1A WGM11:10 = 00, TCCR1B WGM13:12 = 00.
 * Normal mode: count from 0 to 65535, then overflow.
 */
static inline void timer1_set_mode_normal(void)
{
    TCCR1A &= ~((1 << WGM10) | (1 << WGM11));
    TCCR1B &= ~((1 << WGM13) | (1 << WGM12));
}

/*
 * WGM13:10 = 0100: CTC mode 4.
 * OCR1A = TOP: counter resets to 0 after reaching TOP.
 */
static inline void timer1_set_mode_ctc(void)
{
    timer1_set_mode_normal();
    TCCR1B |= (1 << WGM12);
}

/*
 * WGM13:10 = 1110: Fast PWM mode 14.
 * ICR1 = value.
 */
static inline void timer1_set_mode_fast_pwm_top_icr(uint16_t value)
{
    timer1_set_mode_normal();
    TCCR1A |= (1 << WGM11);
    TCCR1B |= (1 << WGM13) | (1 << WGM12);
    ICR1 = value;
}

/*
 * Generate Compare Output Mode functions for OC1A/OC1B.
 *
 * TCCR1A COM1x1:0 (non-PWM mode):
 * 00 = disconnect OC1x, normal port operation.
 * 01 = toggle OC1x on compare match.
 * 10 = clear OC1x on compare match.
 * 11 = set OC1x on compare match.
 */
#define DEFINE_TIMER1_COMPARE_OUTPUT_MODE_ROUTINE(NAME, REGISTER) \
    /* COM1x1:0 = 00: disconnect OC1x from Timer1. */ \
    static inline void timer1_set_com_disconnect_oc1##NAME(void) \
    { \
        TCCR1A &= ~((1 << COM1##REGISTER##1) | \
                    (1 << COM1##REGISTER##0)); \
    } \
    \
    /* COM1x1:0 = 01: toggle OC1x on compare match. */ \
    static inline void timer1_set_com_toggle_oc1##NAME(void) \
    { \
        TCCR1A &= ~(1 << COM1##REGISTER##1); \
        TCCR1A |= (1 << COM1##REGISTER##0); \
    } \
    \
    /* COM1x1:0 = 10: clear OC1x on compare match. */ \
    static inline void timer1_set_com_clear_oc1##NAME(void) \
    { \
        TCCR1A &= ~(1 << COM1##REGISTER##0); \
        TCCR1A |= (1 << COM1##REGISTER##1); \
    } \
    \
    /* COM1x1:0 = 11: set OC1x on compare match. */ \
    static inline void timer1_set_com_set_oc1##NAME(void) \
    { \
        TCCR1A |= (1 << COM1##REGISTER##0) | \
                  (1 << COM1##REGISTER##1); \
    }

DEFINE_TIMER1_COMPARE_OUTPUT_MODE_ROUTINE(a, A)
DEFINE_TIMER1_COMPARE_OUTPUT_MODE_ROUTINE(b, B)

#undef DEFINE_TIMER1_COMPARE_OUTPUT_MODE_ROUTINE

/*
 * TCCR1A COM1A1:0 = 10: non-inverting Fast PWM on OC1A.
 * OC1A = HIGH at BOTTOM, LOW on Compare Match A.
 * DDRB DDB1 = 1 required: PB1 (OC1A) configured as output.
 */
static inline void timer1_set_pwm_non_inverting_oc1a(void)
{
    TCCR1A &= ~(1 << COM1A0);
    TCCR1A |= (1 << COM1A1);
}

#endif
