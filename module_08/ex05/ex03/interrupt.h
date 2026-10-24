#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <avr/interrupt.h>
#include <avr/io.h>

static inline void interrupt_global_enable(void)
{
    __asm__ __volatile__("sei" ::: "memory");
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

#define INTERRUPT_DEFINE_PCINT_GROUP(N)                                                                                                                                                                \
    static inline void interrupt_pcint_group##N##_enable(void)                                                                                                                                         \
    {                                                                                                                                                                                                  \
        PCICR |= (1 << PCIE##N);                                                                                                                                                                       \
    }                                                                                                                                                                                                  \
                                                                                                                                                                                                       \
    static inline void interrupt_pcint_group##N##_disable(void)                                                                                                                                        \
    {                                                                                                                                                                                                  \
        PCICR &= ~(1 << PCIE##N);                                                                                                                                                                      \
    }

INTERRUPT_DEFINE_PCINT_GROUP(2)

#undef INTERRUPT_DEFINE_PCINT_GROUP

#define INTERRUPT_DEFINE_PCINT_PIN(GROUP, PORT, PIN, N)                                                                                                                                                      \
    static inline void interrupt_##PORT##PIN##_enable(void)                                                                                                                                                  \
    {                                                                                                                                                                                                  \
        PCMSK##GROUP |= (1 << PCINT##N);                                                                                                                                                               \
    }                                                                                                                                                                                                  \
                                                                                                                                                                                                       \
    static inline void interrupt_##PORT##PIN##_disable(void)                                                                                                                                              \
    {                                                                                                                                                                                                  \
        PCMSK##GROUP &= ~(1 << PCINT##N);                                                                                                                                                              \
    }

INTERRUPT_DEFINE_PCINT_PIN(2, pd, 0, 16)
INTERRUPT_DEFINE_PCINT_PIN(2, pd, 1, 17)
INTERRUPT_DEFINE_PCINT_PIN(2, pd, 2, 18)
INTERRUPT_DEFINE_PCINT_PIN(2, pd, 3, 19)
INTERRUPT_DEFINE_PCINT_PIN(2, pd, 4, 20)
INTERRUPT_DEFINE_PCINT_PIN(2, pd, 5, 21)
INTERRUPT_DEFINE_PCINT_PIN(2, pd, 6, 22)
INTERRUPT_DEFINE_PCINT_PIN(2, pd, 7, 23)

#undef INTERRUPT_DEFINE_PCINT_PIN

#endif /* INTERRUPT_H */