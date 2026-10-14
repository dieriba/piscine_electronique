#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <avr/interrupt.h>
#include <avr/io.h>

static inline void interrupt_global_enable(void)
{
    sei();
}

static inline void interrupt_global_disable(void)
{
    cli();
}

#define INTERRUPT_DEFINE_EXT(N)                                                                                                                                                                        \
    static inline void interrupt_int##N##_enable(void)                                                                                                                                                 \
    {                                                                                                                                                                                                  \
        EIMSK |= (1 << INT##N);                                                                                                                                                                        \
    }                                                                                                                                                                                                  \
                                                                                                                                                                                                       \
    static inline void interrupt_int##N##_disable(void)                                                                                                                                                \
    {                                                                                                                                                                                                  \
        EIMSK &= (uint8_t) ~(1 << INT##N);                                                                                                                                                             \
    }                                                                                                                                                                                                  \
                                                                                                                                                                                                       \
    static inline void interrupt_int##N##_set_low_level(void)                                                                                                                                          \
    {                                                                                                                                                                                                  \
        EICRA &= (uint8_t) ~((1 << ISC##N##1) | (1 << ISC##N##0));                                                                                                                                     \
    }                                                                                                                                                                                                  \
                                                                                                                                                                                                       \
    static inline void interrupt_int##N##_set_change(void)                                                                                                                                             \
    {                                                                                                                                                                                                  \
        EICRA = (EICRA & (uint8_t) ~(1 << ISC##N##1)) | (1 << ISC##N##0);                                                                                                                              \
    }                                                                                                                                                                                                  \
                                                                                                                                                                                                       \
    static inline void interrupt_int##N##_set_falling_edge(void)                                                                                                                                       \
    {                                                                                                                                                                                                  \
        EICRA = (EICRA & (uint8_t) ~(1 << ISC##N##0)) | (1 << ISC##N##1);                                                                                                                              \
    }                                                                                                                                                                                                  \
                                                                                                                                                                                                       \
    static inline void interrupt_int##N##_set_rising_edge(void)                                                                                                                                        \
    {                                                                                                                                                                                                  \
        EICRA |= (1 << ISC##N##1) | (1 << ISC##N##0);                                                                                                                                                  \
    }

INTERRUPT_DEFINE_EXT(0)
INTERRUPT_DEFINE_EXT(1)

#undef INTERRUPT_DEFINE_EXT

#endif