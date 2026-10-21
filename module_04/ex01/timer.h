#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>
#include <stdint.h>

#define TIMER_GEN_SET_PRESCALER_ROUTINE(TIMER, PRESCALER, CS_VALUE)                                                                                                                                    \
    /* Set CSn2:0 to CS_VALUE; select timer clock division by PRESCALER. */                                                                                                                            \
    static inline void timer##TIMER##_set_prescaler_##PRESCALER(void)                                                                                                                                  \
    {                                                                                                                                                                                                  \
        TCCR##TIMER##B = (TCCR##TIMER##B & (uint8_t) ~((1 << CS##TIMER##2) | (1 << CS##TIMER##1) | (1 << CS##TIMER##0))) | (CS_VALUE);                                                                 \
    }

TIMER_GEN_SET_PRESCALER_ROUTINE(0, 1, 1)
TIMER_GEN_SET_PRESCALER_ROUTINE(0, 8, 2)
TIMER_GEN_SET_PRESCALER_ROUTINE(0, 64, 3)
TIMER_GEN_SET_PRESCALER_ROUTINE(0, 256, 4)
TIMER_GEN_SET_PRESCALER_ROUTINE(0, 1024, 5)

TIMER_GEN_SET_PRESCALER_ROUTINE(1, 1, 1)
TIMER_GEN_SET_PRESCALER_ROUTINE(1, 8, 2)
TIMER_GEN_SET_PRESCALER_ROUTINE(1, 64, 3)
TIMER_GEN_SET_PRESCALER_ROUTINE(1, 256, 4)
TIMER_GEN_SET_PRESCALER_ROUTINE(1, 1024, 5)

TIMER_GEN_SET_PRESCALER_ROUTINE(2, 1, 1)
TIMER_GEN_SET_PRESCALER_ROUTINE(2, 8, 2)
TIMER_GEN_SET_PRESCALER_ROUTINE(2, 32, 3)
TIMER_GEN_SET_PRESCALER_ROUTINE(2, 64, 4)
TIMER_GEN_SET_PRESCALER_ROUTINE(2, 128, 5)
TIMER_GEN_SET_PRESCALER_ROUTINE(2, 256, 6)
TIMER_GEN_SET_PRESCALER_ROUTINE(2, 1024, 7)

#undef TIMER_GEN_SET_PRESCALER_ROUTINE

/* Timer0/2 are 8-bit; OCRnA and OCRnB set compare thresholds. */
#define TIMER_GEN_SET_COMPARE_REGISTER(TIMER)                                                                                                                                                          \
    /* Write OCRnA; set channel A compare threshold (or TOP in selected modes). */                                                                                                                     \
    static inline void timer##TIMER##_set_compare_a(uint8_t value)                                                                                                                                     \
    {                                                                                                                                                                                                  \
        OCR##TIMER##A = value;                                                                                                                                                                         \
    }                                                                                                                                                                                                  \
    /* Write OCRnB; set channel B compare threshold. */                                                                                                                                                \
    static inline void timer##TIMER##_set_compare_b(uint8_t value)                                                                                                                                     \
    {                                                                                                                                                                                                  \
        OCR##TIMER##B = value;                                                                                                                                                                         \
    }

TIMER_GEN_SET_COMPARE_REGISTER(0)
TIMER_GEN_SET_COMPARE_REGISTER(2)
#undef TIMER_GEN_SET_COMPARE_REGISTER

#define TIMER_GEN_SET_8BIT_WGM_ROUTINES(TIMER)                                                                                                                                                         \
    /* WGM = 0: count up to maximum, then overflow. */                                                                                                                                                 \
    static inline void timer##TIMER##_set_mode_normal(void)                                                                                                                                            \
    {                                                                                                                                                                                                  \
        TCCR##TIMER##A &= (uint8_t) ~((1 << WGM##TIMER##1) | (1 << WGM##TIMER##0));                                                                                                                    \
        TCCR##TIMER##B &= (uint8_t) ~(1 << WGM##TIMER##2);                                                                                                                                             \
    }                                                                                                                                                                                                  \
    /* WGM = 2 (8-bit) or 4 (Timer1): clear counter at OCRnA. */                                                                                                                                       \
    static inline void timer##TIMER##_set_mode_ctc(void)                                                                                                                                               \
    {                                                                                                                                                                                                  \
        timer##TIMER##_set_mode_normal();                                                                                                                                                              \
        TCCR##TIMER##A |= (1 << WGM##TIMER##1);                                                                                                                                                        \
    }                                                                                                                                                                                                  \
    /* WGM = 3: Fast PWM, TOP = 0xFF. */                                                                                                                                                               \
    static inline void timer##TIMER##_set_mode_fast_pwm_top_ff(void)                                                                                                                                   \
    {                                                                                                                                                                                                  \
        timer##TIMER##_set_mode_normal();                                                                                                                                                              \
        TCCR##TIMER##A |= (1 << WGM##TIMER##1) | (1 << WGM##TIMER##0);                                                                                                                                 \
    }                                                                                                                                                                                                  \
    /* WGM = 1: Phase Correct PWM, TOP = 0xFF. */                                                                                                                                                      \
    static inline void timer##TIMER##_set_mode_phase_correct_pwm_top_ff(void)                                                                                                                          \
    {                                                                                                                                                                                                  \
        timer##TIMER##_set_mode_normal();                                                                                                                                                              \
        TCCR##TIMER##A |= (1 << WGM##TIMER##0);                                                                                                                                                        \
    }                                                                                                                                                                                                  \
    /* WGM = 7: Fast PWM, TOP = OCRnA. */                                                                                                                                                              \
    static inline void timer##TIMER##_set_mode_fast_pwm_top_reg_a(void)                                                                                                                                \
    {                                                                                                                                                                                                  \
        timer##TIMER##_set_mode_fast_pwm_top_ff();                                                                                                                                                     \
        TCCR##TIMER##B |= (1 << WGM##TIMER##2);                                                                                                                                                        \
    }                                                                                                                                                                                                  \
    /* WGM = 5: Phase Correct PWM, TOP = OCRnA. */                                                                                                                                                     \
    static inline void timer##TIMER##_set_mode_phase_correct_pwm_top_reg_a(void)                                                                                                                       \
    {                                                                                                                                                                                                  \
        timer##TIMER##_set_mode_phase_correct_pwm_top_ff();                                                                                                                                            \
        TCCR##TIMER##B |= (1 << WGM##TIMER##2);                                                                                                                                                        \
    }

TIMER_GEN_SET_8BIT_WGM_ROUTINES(0)
TIMER_GEN_SET_8BIT_WGM_ROUTINES(2)
#undef TIMER_GEN_SET_8BIT_WGM_ROUTINES

#define TIMER_GEN_COM_OUTPUT_ROUTINES(TIMER, CHANNEL, CHANNEL_UPPER)                                                                                                                                   \
    /* COMnx1:0 = 00: disconnect OCnx; use normal GPIO. */                                                                                                                                             \
    static inline void timer##TIMER##_set_compare_output_mode_disconnect_oc##TIMER##CHANNEL(void)                                                                                                      \
    {                                                                                                                                                                                                  \
        TCCR##TIMER##A &= (uint8_t) ~((1 << COM##TIMER##CHANNEL_UPPER##1) | (1 << COM##TIMER##CHANNEL_UPPER##0));                                                                                      \
    }                                                                                                                                                                                                  \
    /* COMnx1:0 = 01: toggle OCnx on compare match in non-PWM mode. */                                                                                                                                 \
    static inline void timer##TIMER##_set_compare_output_mode_non_pwm_toggle_oc##TIMER##CHANNEL(void)                                                                                                  \
    {                                                                                                                                                                                                  \
        TCCR##TIMER##A = (TCCR##TIMER##A & (uint8_t) ~(1 << COM##TIMER##CHANNEL_UPPER##1)) | (1 << COM##TIMER##CHANNEL_UPPER##0);                                                                      \
    }                                                                                                                                                                                                  \
    /* COMnx1:0 = 10: clear OCnx on compare match in non-PWM mode. */                                                                                                                                  \
    static inline void timer##TIMER##_set_compare_output_mode_non_pwm_clear_oc##TIMER##CHANNEL(void)                                                                                                   \
    {                                                                                                                                                                                                  \
        TCCR##TIMER##A = (TCCR##TIMER##A & (uint8_t) ~(1 << COM##TIMER##CHANNEL_UPPER##0)) | (1 << COM##TIMER##CHANNEL_UPPER##1);                                                                      \
    }                                                                                                                                                                                                  \
    /* COMnx1:0 = 11: set OCnx on compare match in non-PWM mode. */                                                                                                                                    \
    static inline void timer##TIMER##_set_compare_output_mode_non_pwm_set_oc##TIMER##CHANNEL(void)                                                                                                     \
    {                                                                                                                                                                                                  \
        TCCR##TIMER##A |= (1 << COM##TIMER##CHANNEL_UPPER##1) | (1 << COM##TIMER##CHANNEL_UPPER##0);                                                                                                   \
    }                                                                                                                                                                                                  \
    /* COMnx1:0 = 10: set at BOTTOM, clear on match (Fast PWM). */                                                                                                                                     \
    static inline void timer##TIMER##_set_compare_output_mode_fast_pwm_set_at_bottom_clear_on_match_oc##TIMER##CHANNEL(void)                                                                           \
    {                                                                                                                                                                                                  \
        TCCR##TIMER##A = (TCCR##TIMER##A & (uint8_t) ~(1 << COM##TIMER##CHANNEL_UPPER##0)) | (1 << COM##TIMER##CHANNEL_UPPER##1);                                                                      \
    }                                                                                                                                                                                                  \
    /* COMnx1:0 = 11: clear at BOTTOM, set on match (Fast PWM). */                                                                                                                                     \
    static inline void timer##TIMER##_set_compare_output_mode_fast_pwm_clear_at_bottom_set_on_match_oc##TIMER##CHANNEL(void)                                                                           \
    {                                                                                                                                                                                                  \
        TCCR##TIMER##A |= (1 << COM##TIMER##CHANNEL_UPPER##1) | (1 << COM##TIMER##CHANNEL_UPPER##0);                                                                                                   \
    }                                                                                                                                                                                                  \
    /* COMnx1:0 = 10: clear on up-count match, set on down-count match. */                                                                                                                             \
    static inline void timer##TIMER##_set_compare_output_mode_phase_correct_pwm_clear_on_upcount_set_on_downcount_oc##TIMER##CHANNEL(void)                                                             \
    {                                                                                                                                                                                                  \
        TCCR##TIMER##A = (TCCR##TIMER##A & (uint8_t) ~(1 << COM##TIMER##CHANNEL_UPPER##0)) | (1 << COM##TIMER##CHANNEL_UPPER##1);                                                                      \
    }                                                                                                                                                                                                  \
    /* COMnx1:0 = 11: set on up-count match, clear on down-count match. */                                                                                                                             \
    static inline void timer##TIMER##_set_compare_output_mode_phase_correct_pwm_set_on_upcount_clear_on_downcount_oc##TIMER##CHANNEL(void)                                                             \
    {                                                                                                                                                                                                  \
        TCCR##TIMER##A |= (1 << COM##TIMER##CHANNEL_UPPER##1) | (1 << COM##TIMER##CHANNEL_UPPER##0);                                                                                                   \
    }

TIMER_GEN_COM_OUTPUT_ROUTINES(0, a, A)
TIMER_GEN_COM_OUTPUT_ROUTINES(0, b, B)
TIMER_GEN_COM_OUTPUT_ROUTINES(1, a, A)
TIMER_GEN_COM_OUTPUT_ROUTINES(1, b, B)
TIMER_GEN_COM_OUTPUT_ROUTINES(2, a, A)
TIMER_GEN_COM_OUTPUT_ROUTINES(2, b, B)
#undef TIMER_GEN_COM_OUTPUT_ROUTINES

/*
 * Generate Timer N interrupt selection functions.
 *
 * TIMSKn : Timer/Counter Interrupt Mask Register.
 * TOIEn  : 1 = enable overflow interrupt, 0 = disable.
 * OCIEnA : 1 = enable compare match A interrupt, 0 = disable.
 * OCIEnB : 1 = enable compare match B interrupt, 0 = disable.
 *
 * Only one of these three interrupts is enabled at a time.
 */
#define TIMER_GEN_SET_ON_MATCH_INTERRUPT_ROUTINE(TIMER)                                                                                                                                                \
                                                                                                                                                                                                       \
    /* Enable overflow interrupt; disable compare match A/B. */                                                                                                                                        \
    static inline void timer##TIMER##_set_interrupt_on_overflow(void)                                                                                                                                  \
    {                                                                                                                                                                                                  \
        TIMSK##TIMER &= ~((1 << OCIE##TIMER##B) | (1 << OCIE##TIMER##A));                                                                                                                              \
        TIMSK##TIMER |= (1 << TOIE##TIMER);                                                                                                                                                            \
    }                                                                                                                                                                                                  \
                                                                                                                                                                                                       \
    /* Enable compare match A interrupt; disable overflow and match B. */                                                                                                                              \
    static inline void timer##TIMER##_set_interrupt_on_match_a(void)                                                                                                                                   \
    {                                                                                                                                                                                                  \
        TIMSK##TIMER &= ~((1 << OCIE##TIMER##B) | (1 << TOIE##TIMER));                                                                                                                                 \
        TIMSK##TIMER |= (1 << OCIE##TIMER##A);                                                                                                                                                         \
    }                                                                                                                                                                                                  \
                                                                                                                                                                                                       \
    /* Enable compare match B interrupt; disable overflow and match A. */                                                                                                                              \
    static inline void timer##TIMER##_set_interrupt_on_match_b(void)                                                                                                                                   \
    {                                                                                                                                                                                                  \
        TIMSK##TIMER &= ~((1 << OCIE##TIMER##A) | (1 << TOIE##TIMER));                                                                                                                                 \
        TIMSK##TIMER |= (1 << OCIE##TIMER##B);                                                                                                                                                         \
    }

/* Generate functions for Timer0, Timer1 and Timer2. */
TIMER_GEN_SET_ON_MATCH_INTERRUPT_ROUTINE(0)
TIMER_GEN_SET_ON_MATCH_INTERRUPT_ROUTINE(1)
TIMER_GEN_SET_ON_MATCH_INTERRUPT_ROUTINE(2)

#undef TIMER_GEN_SET_ON_MATCH_INTERRUPT_ROUTINE

/* Timer1: 16-bit compare registers and input capture TOP. */
/* Write OCRnA; set channel A compare threshold (or TOP in selected modes). */
static inline void timer1_set_compare_a(uint16_t value)
{
    OCR1A = value;
}

/* Write OCRnB; set channel B compare threshold. */
static inline void timer1_set_compare_b(uint16_t value)
{
    OCR1B = value;
}

/* Write ICR1; select TOP for Timer1 modes using ICR1. */
static inline void timer1_set_top_icr(uint16_t value)
{
    ICR1 = value;
}

/* Set OCR1A threshold and enable compare A interrupt. */
static inline void timer1_interrupt_on_match_a(uint16_t value)
{
    timer1_set_compare_a(value);
    timer1_set_interrupt_on_match_a();
}

/* Timer1 WGM13:10 = 0000: Normal, TOP=0xFFFF. */
/* WGM = 0: count up to maximum, then overflow. */
static inline void timer1_set_mode_normal(void)
{
    TCCR1A &= (uint8_t) ~((1 << WGM11) | (1 << WGM10));
    TCCR1B &= (uint8_t) ~((1 << WGM13) | (1 << WGM12));
}

/* Timer1 WGM13:10 = 0100: CTC, TOP=OCR1A. */
/* WGM = 2 (8-bit) or 4 (Timer1): clear counter at OCRnA. */
static inline void timer1_set_mode_ctc(void)
{
    timer1_set_mode_normal();
    TCCR1B |= (1 << WGM12);
}

/* Timer1 WGM13:10 = 1110: Fast PWM, TOP=ICR1. */
/* WGM13:10 = 1110: Fast PWM, TOP = ICR1 (value). */
static inline void timer1_set_mode_fast_pwm_top_icr(uint16_t value)
{
    timer1_set_mode_normal();
    TCCR1A |= (1 << WGM11);
    TCCR1B |= (1 << WGM13) | (1 << WGM12);
    timer1_set_top_icr(value);
}

#endif
