#ifndef AVR_PORT_UTILS_H
#define AVR_PORT_UTILS_H

#define LOW 0
#define HIGH 1
#include <avr/io.h>
#include <stdint.h>

/*
 * Generate GPIO functions for pin N of port P.
 *
 * DDRx  : 1 = output, 0 = input.
 * PORTx : 1 = HIGH / pull-up enabled, 0 = LOW / pull-up disabled.
 * PINx  : reads pin state; writing 1 toggles the PORTx bit.
 */
#define GPIO_OP(name, P, N)                                                                                                                                                                            \
                                                                                                                                                                                                       \
    /* Toggle pin N by writing 1 to PINx bit N. */                                                                                                                                                     \
    static inline void name##_toggle(void)                                                                                                                                                             \
    {                                                                                                                                                                                                  \
        PIN##P = (1 << (N));                                                                                                                                                                           \
    }                                                                                                                                                                                                  \
                                                                                                                                                                                                       \
    /* Read PINx bit N: returns 0 (LOW) or 1 (HIGH). */                                                                                                                                                \
    static inline uint8_t name##_read(void)                                                                                                                                                            \
    {                                                                                                                                                                                                  \
        return (PIN##P >> (N)) & 1;                                                                                                                                                                    \
    }                                                                                                                                                                                                  \
                                                                                                                                                                                                       \
    /* Set DDRx bit N to 1: configure pin as output. */                                                                                                                                                \
    static inline void name##_output(void)                                                                                                                                                             \
    {                                                                                                                                                                                                  \
        DDR##P |= (1 << (N));                                                                                                                                                                          \
    }                                                                                                                                                                                                  \
                                                                                                                                                                                                       \
    /* Clear DDRx bit N to 0: configure pin as input. */                                                                                                                                               \
    static inline void name##_input(void)                                                                                                                                                              \
    {                                                                                                                                                                                                  \
        DDR##P &= ~(1 << (N));                                                                                                                                                                         \
    }                                                                                                                                                                                                  \
                                                                                                                                                                                                       \
    /* Set PORTx bit N to 1: HIGH or enable pull-up. */                                                                                                                                                \
    static inline void name##_high(void)                                                                                                                                                               \
    {                                                                                                                                                                                                  \
        PORT##P |= (1 << (N));                                                                                                                                                                         \
    }                                                                                                                                                                                                  \
                                                                                                                                                                                                       \
    /* Clear PORTx bit N to 0: LOW or disable pull-up. */                                                                                                                                              \
    static inline void name##_low(void)                                                                                                                                                                \
    {                                                                                                                                                                                                  \
        PORT##P &= ~(1 << (N));                                                                                                                                                                        \
    }                                                                                                                                                                                                  \
                                                                                                                                                                                                       \
    /* DDRx bit N = 0, PORTx bit N = 1: enable pull-up. */                                                                                                                                             \
    static inline void name##_pull_up_enable(void)                                                                                                                                                     \
    {                                                                                                                                                                                                  \
        name##_input();                                                                                                                                                                                \
        name##_high();                                                                                                                                                                                 \
    }                                                                                                                                                                                                  \
                                                                                                                                                                                                       \
    /* PORTx bit N = 0: disable pull-up (input mode). */                                                                                                                                               \
    static inline void name##_pull_up_disable(void)                                                                                                                                                    \
    {                                                                                                                                                                                                  \
        name##_low();                                                                                                                                                                                  \
    }

/* PORT B */
GPIO_OP(pb0, B, 0)
GPIO_OP(pb1, B, 1)
GPIO_OP(pb2, B, 2)
GPIO_OP(pb3, B, 3)
GPIO_OP(pb4, B, 4)
GPIO_OP(pb5, B, 5)

/* PORT C */
GPIO_OP(pc0, C, 0)
GPIO_OP(pc1, C, 1)
GPIO_OP(pc2, C, 2)
GPIO_OP(pc3, C, 3)
GPIO_OP(pc4, C, 4)
GPIO_OP(pc5, C, 5)

/* PORT D */
GPIO_OP(pd0, D, 0)
GPIO_OP(pd1, D, 1)
GPIO_OP(pd2, D, 2)
GPIO_OP(pd3, D, 3)
GPIO_OP(pd4, D, 4)
GPIO_OP(pd5, D, 5)
GPIO_OP(pd6, D, 6)
GPIO_OP(pd7, D, 7)

#endif