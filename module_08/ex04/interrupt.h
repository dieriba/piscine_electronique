#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <avr/interrupt.h>
#include <avr/io.h>

/*
 * Control global interrupts.
 *
 * SREG : I = 1 enables all unmasked interrupts (sei).
 *        I = 0 disables all maskable interrupts (cli).
 */

/* Set SREG I bit to 1: enable global interrupts. */
static inline void interrupt_global_enable(void)
{
    __asm__ __volatile__("sei" ::: "memory");
}

/* Clear SREG I bit to 0: disable global interrupts. */
static inline void interrupt_global_disable(void)
{
    cli();
}

/*
 * Generate external interrupt INTn functions.
 *
 * EIMSK : INTn = 1 enables external interrupt INTn.
 *         INTn = 0 disables external interrupt INTn.
 *
 * EICRA : ISCn1:0 = 00 -> low level.
 *                  = 01 -> any logical change.
 *                  = 10 -> falling edge.
 *                  = 11 -> rising edge.
 */
#define INTERRUPT_DEFINE_EXT(N)                                                                                                                                                                        \
                                                                                                                                                                                                       \
    /* Set EIMSK INTn bit to 1: enable external interrupt. */                                                                                                                                          \
    static inline void interrupt_int##N##_enable(void)                                                                                                                                                 \
    {                                                                                                                                                                                                  \
        EIMSK |= (1 << INT##N);                                                                                                                                                                        \
    }                                                                                                                                                                                                  \
                                                                                                                                                                                                       \
    /* Clear EIMSK INTn bit to 0: disable external interrupt. */                                                                                                                                       \
    static inline void interrupt_int##N##_disable(void)                                                                                                                                                \
    {                                                                                                                                                                                                  \
        EIMSK &= (uint8_t) ~(1 << INT##N);                                                                                                                                                             \
    }                                                                                                                                                                                                  \
                                                                                                                                                                                                       \
    /* ISCn1:0 = 00: trigger while input is LOW. */                                                                                                                                                    \
    static inline void interrupt_int##N##_set_low_level(void)                                                                                                                                          \
    {                                                                                                                                                                                                  \
        EICRA &= (uint8_t) ~((1 << ISC##N##1) | (1 << ISC##N##0));                                                                                                                                     \
    }                                                                                                                                                                                                  \
                                                                                                                                                                                                       \
    /* ISCn1:0 = 01: trigger on any logical change. */                                                                                                                                                 \
    static inline void interrupt_int##N##_set_change(void)                                                                                                                                             \
    {                                                                                                                                                                                                  \
        EICRA = (EICRA & (uint8_t) ~(1 << ISC##N##1)) | (1 << ISC##N##0);                                                                                                                              \
    }                                                                                                                                                                                                  \
                                                                                                                                                                                                       \
    /* ISCn1:0 = 10: trigger on falling edge. */                                                                                                                                                       \
    static inline void interrupt_int##N##_set_falling_edge(void)                                                                                                                                       \
    {                                                                                                                                                                                                  \
        EICRA = (EICRA & (uint8_t) ~(1 << ISC##N##0)) | (1 << ISC##N##1);                                                                                                                              \
    }                                                                                                                                                                                                  \
                                                                                                                                                                                                       \
    /* ISCn1:0 = 11: trigger on rising edge. */                                                                                                                                                        \
    static inline void interrupt_int##N##_set_rising_edge(void)                                                                                                                                        \
    {                                                                                                                                                                                                  \
        EICRA |= (1 << ISC##N##1) | (1 << ISC##N##0);                                                                                                                                                  \
    }

/* External interrupts INT0 (PD2) and INT1 (PD3). */
INTERRUPT_DEFINE_EXT(0)
INTERRUPT_DEFINE_EXT(1)

#undef INTERRUPT_DEFINE_EXT

/*
 * Generate pin change interrupt group functions.
 *
 * PCICR : PCIEn = 1 enables pin change interrupt group n.
 *         PCIEn = 0 disables pin change interrupt group n.
 *
 * Group 0 : PCINT0-7   (PORTB).
 * Group 1 : PCINT8-14  (PORTC).
 * Group 2 : PCINT16-23 (PORTD).
 */
#define INTERRUPT_DEFINE_PCINT_GROUP(N)                                                                                                                                                                \
                                                                                                                                                                                                       \
    /* Set PCICR PCIEn bit to 1: enable pin change interrupt group. */                                                                                                                                 \
    static inline void interrupt_pcint_group##N##_enable(void)                                                                                                                                         \
    {                                                                                                                                                                                                  \
        PCICR |= (1 << PCIE##N);                                                                                                                                                                       \
    }                                                                                                                                                                                                  \
                                                                                                                                                                                                       \
    /* Clear PCICR PCIEn bit to 0: disable pin change interrupt group. */                                                                                                                              \
    static inline void interrupt_pcint_group##N##_disable(void)                                                                                                                                        \
    {                                                                                                                                                                                                  \
        PCICR &= ~(1 << PCIE##N);                                                                                                                                                                      \
    }

/* Generate pin change interrupt group 2 (PORTD). */
INTERRUPT_DEFINE_PCINT_GROUP(2)

#undef INTERRUPT_DEFINE_PCINT_GROUP

/*
 * Generate individual pin change interrupt functions.
 *
 * PCMSKn : PCINTx = 1 enables pin change detection.
 *          PCINTx = 0 disables pin change detection.
 *
 * Interrupt triggers on any logical change (LOW <-> HIGH).
 */
#define INTERRUPT_DEFINE_PCINT_PIN(GROUP, PORT, PIN, N)                                                                                                                                                \
                                                                                                                                                                                                       \
    /* Set PCMSKn PCINTx bit to 1: enable pin change detection. */                                                                                                                                     \
    static inline void interrupt_##PORT##PIN##_enable(void)                                                                                                                                            \
    {                                                                                                                                                                                                  \
        PCMSK##GROUP |= (1 << PCINT##N);                                                                                                                                                               \
    }                                                                                                                                                                                                  \
                                                                                                                                                                                                       \
    /* Clear PCMSKn PCINTx bit to 0: disable pin change detection. */                                                                                                                                  \
    static inline void interrupt_##PORT##PIN##_disable(void)                                                                                                                                           \
    {                                                                                                                                                                                                  \
        PCMSK##GROUP &= ~(1 << PCINT##N);                                                                                                                                                              \
    }

/* PORTD: individual pin change interrupts PCINT16-23. */
INTERRUPT_DEFINE_PCINT_PIN(2, pd, 0, 16)
INTERRUPT_DEFINE_PCINT_PIN(2, pd, 1, 17)
INTERRUPT_DEFINE_PCINT_PIN(2, pd, 2, 18)
INTERRUPT_DEFINE_PCINT_PIN(2, pd, 3, 19)
INTERRUPT_DEFINE_PCINT_PIN(2, pd, 4, 20)
INTERRUPT_DEFINE_PCINT_PIN(2, pd, 5, 21)
INTERRUPT_DEFINE_PCINT_PIN(2, pd, 6, 22)
INTERRUPT_DEFINE_PCINT_PIN(2, pd, 7, 23)

#undef INTERRUPT_DEFINE_PCINT_PIN

#endif