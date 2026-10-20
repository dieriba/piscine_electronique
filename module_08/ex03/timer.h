#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>
#include <stdint.h>

/* Set CSn2:0 while preserving other TCCRnB bits.
 * Timer0/1: 0=stop, 1=/1, 2=/8, 3=/64, 4=/256, 5=/1024.
 * Timer2:   0=stop, 1=/1, 2=/8, 3=/32, 4=/64,
 *           5=/128, 6=/256, 7=/1024.
 */
#define TIMER_GEN_SET_PRESCALER_ROUTINE(TIMER, PRESCALER, N)                                                                                                                                           \
    /* CSn2:0 = N: select timer clock divisor PRESCALER. */                                                                                                                                            \
    static inline void timer##TIMER##_set_prescaler_##PRESCALER(void)                                                                                                                                  \
    {                                                                                                                                                                                                  \
        TCCR##TIMER##B = (TCCR##TIMER##B & ~((1 << CS##TIMER##2) | (1 << CS##TIMER##1) | (1 << CS##TIMER##0))) | ((N) << CS##TIMER##0);                                                                \
    }

TIMER_GEN_SET_PRESCALER_ROUTINE(0, 0, 0)
TIMER_GEN_SET_PRESCALER_ROUTINE(0, 1, 1)
TIMER_GEN_SET_PRESCALER_ROUTINE(0, 8, 2)
TIMER_GEN_SET_PRESCALER_ROUTINE(0, 64, 3)
TIMER_GEN_SET_PRESCALER_ROUTINE(0, 256, 4)
TIMER_GEN_SET_PRESCALER_ROUTINE(0, 1024, 5)

TIMER_GEN_SET_PRESCALER_ROUTINE(1, 0, 0)
TIMER_GEN_SET_PRESCALER_ROUTINE(1, 1, 1)
TIMER_GEN_SET_PRESCALER_ROUTINE(1, 8, 2)
TIMER_GEN_SET_PRESCALER_ROUTINE(1, 64, 3)
TIMER_GEN_SET_PRESCALER_ROUTINE(1, 256, 4)
TIMER_GEN_SET_PRESCALER_ROUTINE(1, 1024, 5)

TIMER_GEN_SET_PRESCALER_ROUTINE(2, 0, 0)
TIMER_GEN_SET_PRESCALER_ROUTINE(2, 1, 1)
TIMER_GEN_SET_PRESCALER_ROUTINE(2, 8, 2)
TIMER_GEN_SET_PRESCALER_ROUTINE(2, 32, 3)
TIMER_GEN_SET_PRESCALER_ROUTINE(2, 64, 4)
TIMER_GEN_SET_PRESCALER_ROUTINE(2, 128, 5)
TIMER_GEN_SET_PRESCALER_ROUTINE(2, 256, 6)
TIMER_GEN_SET_PRESCALER_ROUTINE(2, 1024, 7)

#undef TIMER_GEN_SET_PRESCALER_ROUTINE

/* Timer0/Timer2: 8-bit compare registers, Fast PWM and overflow IRQ. */
#define TIMER_GEN_8_BITS_TIMER_ROUTINE(TIMER)                                                                                                                                                          \
    /* OCRnA = value: set Compare Match A threshold. */                                                                                                                                                \
    static inline void timer##TIMER##_set_reg_a(uint8_t value)                                                                                                                                         \
    {                                                                                                                                                                                                  \
        OCR##TIMER##A = value;                                                                                                                                                                         \
    }                                                                                                                                                                                                  \
    /* OCRnB = value: set Compare Match B threshold. */                                                                                                                                                \
    static inline void timer##TIMER##_set_reg_b(uint8_t value)                                                                                                                                         \
    {                                                                                                                                                                                                  \
        OCR##TIMER##B = value;                                                                                                                                                                         \
    }                                                                                                                                                                                                  \
    /* Fast PWM, TOP = 0xFF (WGMn2:0 = 011). */                                                                                                                                                        \
    static inline void timer##TIMER##_set_mode_fast_pwm_top_ff(void)                                                                                                                                   \
    {                                                                                                                                                                                                  \
        TCCR##TIMER##A |= (1 << WGM##TIMER##0) | (1 << WGM##TIMER##1);                                                                                                                                 \
        TCCR##TIMER##B &= ~(1 << WGM##TIMER##2);                                                                                                                                                       \
    }                                                                                                                                                                                                  \
    /* Fast PWM, TOP = OCRnA (WGMn2:0 = 111). */                                                                                                                                                       \
    static inline void timer##TIMER##_set_mode_fast_pwm_top_reg_a(void)                                                                                                                                \
    {                                                                                                                                                                                                  \
        TCCR##TIMER##A |= (1 << WGM##TIMER##0) | (1 << WGM##TIMER##1);                                                                                                                                 \
        TCCR##TIMER##B |= (1 << WGM##TIMER##2);                                                                                                                                                        \
    }                                                                                                                                                                                                  \
    /* Non-inverting PWM on OCnA/OCnB. Configure DDR separately. */                                                                                                                                    \
    /* COMnA1:0 = 10: non-inverting PWM on OCnA. */                                                                                                                                                    \
    static inline void timer##TIMER##_pwm_enable_a(void)                                                                                                                                               \
    {                                                                                                                                                                                                  \
        TCCR##TIMER##A = (TCCR##TIMER##A & ~(1 << COM##TIMER##A0)) | (1 << COM##TIMER##A1);                                                                                                            \
    }                                                                                                                                                                                                  \
    /* COMnB1:0 = 10: non-inverting PWM on OCnB. */                                                                                                                                                    \
    static inline void timer##TIMER##_pwm_enable_b(void)                                                                                                                                               \
    {                                                                                                                                                                                                  \
        TCCR##TIMER##A = (TCCR##TIMER##A & ~(1 << COM##TIMER##B0)) | (1 << COM##TIMER##B1);                                                                                                            \
    }                                                                                                                                                                                                  \
    /* TIMSKn TOIEn = 1: enable overflow interrupt. */                                                                                                                                                 \
    static inline void timer##TIMER##_set_ovf_trigger_isr(void)                                                                                                                                        \
    {                                                                                                                                                                                                  \
        TIMSK##TIMER |= (1 << TOIE##TIMER);                                                                                                                                                            \
    }                                                                                                                                                                                                  \
    /* TIMSKn TOIEn = 0: disable overflow interrupt. */                                                                                                                                                \
    static inline void timer##TIMER##_unset_ovf_trigger_isr(void)                                                                                                                                      \
    {                                                                                                                                                                                                  \
        TIMSK##TIMER &= ~(1 << TOIE##TIMER);                                                                                                                                                           \
    }

TIMER_GEN_8_BITS_TIMER_ROUTINE(0)
TIMER_GEN_8_BITS_TIMER_ROUTINE(2)
#undef TIMER_GEN_8_BITS_TIMER_ROUTINE

/* Timer1: 16-bit compare registers and TOP. */
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

/* Backward-compatible alias. */
static inline void timer1_set_compare_icr(uint16_t value)
{
    timer1_set_top_icr(value);
}

/* TIMSK1 OCIE1A = 1: enable Compare Match A interrupt. */
static inline void timer1_set_interrupt_on_match_a(void)
{
    TIMSK1 |= (1 << OCIE1A);
}

/* OCR1A = value; TIMSK1 OCIE1A = 1: enable Compare Match A interrupt. */
static inline void timer1_interrupt_on_match_a(uint16_t value)
{
    timer1_set_compare_a(value);
    timer1_set_interrupt_on_match_a();
}

/* WGM13:10 = 0000: Normal mode. */
static inline void timer1_set_mode_normal(void)
{
    TCCR1A &= ~((1 << WGM10) | (1 << WGM11));
    TCCR1B &= ~((1 << WGM12) | (1 << WGM13));
}

/* Backward-compatible alias. */
static inline void timer1_set_default_mode(void)
{
    timer1_set_mode_normal();
}

/* WGM13:10 = 0100: CTC, TOP = OCR1A. */
static inline void timer1_set_mode_ctc(void)
{
    timer1_set_mode_normal();
    TCCR1B |= (1 << WGM12);
}

/* WGM13:10 = 1110: Fast PWM mode 14, TOP = ICR1. */
static inline void timer1_set_mode_fast_pwm_top_icr(uint16_t value)
{
    timer1_set_mode_normal();
    TCCR1A |= (1 << WGM11);
    TCCR1B |= (1 << WGM13) | (1 << WGM12);
    timer1_set_top_icr(value);
}

/* Select mode 14 without changing ICR1. */
static inline void timer1_set_mode_fast_pwm_top_icr_preserve(void)
{
    timer1_set_mode_normal();
    TCCR1A |= (1 << WGM11);
    TCCR1B |= (1 << WGM13) | (1 << WGM12);
}

/* WGM13:10 = 1111: Fast PWM mode 15, TOP = OCR1A.
 * OC1A PWM output has special restrictions when OCR1A is TOP.
 */
static inline void timer1_set_mode_fast_pwm_top_oca(void)
{
    timer1_set_mode_normal();
    TCCR1A |= (1 << WGM11) | (1 << WGM10);
    TCCR1B |= (1 << WGM13) | (1 << WGM12);
}

/* Compare output modes: COM1x1:0 = 00/01/10/11.
 * Non-PWM: disconnect/toggle/clear/set on compare match.
 * In PWM modes, COM behavior is mode-dependent.
 */
#define DEFINE_TIMER1_COMPARE_OUTPUT_MODE_ROUTINE(NAME, CHANNEL)                                                                                                                                       \
    /* COM1x1:0 = 00: disconnect OC1x from Timer1. */                                                                                                                                                  \
    static inline void timer1_set_com_disconnect_oc1##NAME(void)                                                                                                                                       \
    {                                                                                                                                                                                                  \
        TCCR1A &= ~((1 << COM1##CHANNEL##1) | (1 << COM1##CHANNEL##0));                                                                                                                                \
    }                                                                                                                                                                                                  \
    /* COM1x1:0 = 01: toggle OC1x on compare match (non-PWM). */                                                                                                                                       \
    static inline void timer1_set_com_toggle_oc1##NAME(void)                                                                                                                                           \
    {                                                                                                                                                                                                  \
        TCCR1A = (TCCR1A & ~(1 << COM1##CHANNEL##1)) | (1 << COM1##CHANNEL##0);                                                                                                                        \
    }                                                                                                                                                                                                  \
    /* COM1x1:0 = 10: clear OC1x on compare match (non-PWM). */                                                                                                                                        \
    static inline void timer1_set_com_clear_oc1##NAME(void)                                                                                                                                            \
    {                                                                                                                                                                                                  \
        TCCR1A = (TCCR1A & ~(1 << COM1##CHANNEL##0)) | (1 << COM1##CHANNEL##1);                                                                                                                        \
    }                                                                                                                                                                                                  \
    /* COM1x1:0 = 11: set OC1x on compare match (non-PWM). */                                                                                                                                          \
    static inline void timer1_set_com_set_oc1##NAME(void)                                                                                                                                              \
    {                                                                                                                                                                                                  \
        TCCR1A |= (1 << COM1##CHANNEL##1) | (1 << COM1##CHANNEL##0);                                                                                                                                   \
    }

DEFINE_TIMER1_COMPARE_OUTPUT_MODE_ROUTINE(a, A)
DEFINE_TIMER1_COMPARE_OUTPUT_MODE_ROUTINE(b, B)
#undef DEFINE_TIMER1_COMPARE_OUTPUT_MODE_ROUTINE

/* Non-inverting Fast PWM on OC1A: HIGH at BOTTOM, LOW on match.
 * On ATmega328P, configure PB1 as an output separately.
 */
static inline void timer1_set_pwm_non_inverting_oc1a(void)
{
    TCCR1A = (TCCR1A & ~(1 << COM1A0)) | (1 << COM1A1);
}

#endif
