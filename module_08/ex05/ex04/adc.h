
#ifndef ADC_H
#define ADC_H

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

/*
 * Enable ADC peripheral.
 *
 * ADCSRA : ADEN = 1 enables ADC, 0 disables ADC.
 */
static inline void adc_enable()
{
    ADCSRA |= (1 << ADEN);
}

/*
 * Enable ADC conversion complete interrupt.
 *
 * ADCSRA : ADIE = 1 enables ADC interrupt.
 *          ADIE = 0 disables ADC interrupt.
 */
static inline void adc_enable_interrupt()
{
    ADCSRA |= (1 << ADIE);
}

/*
 * Generate ADC auto-trigger source functions.
 *
 * ADCSRA : ADATE = 1 enables auto-triggering.
 * ADCSRB : ADTS2:0 selects the auto-trigger source.
 *          101 = Timer1 Compare Match B.
 */
#define ADC_DEFINE_ENABLE_AUTO_TRIGGER_ROUTINE(SRC, VALUE)                    \
                                                                             \
    /* Enable auto-trigger and select trigger source. */                      \
    static inline void adc_enable_auto_trigger_src_##SRC()                    \
    {                                                                        \
        ADCSRA |= (1 << ADATE);                                              \
        ADCSRB |= VALUE;                                                     \
    }

ADC_DEFINE_ENABLE_AUTO_TRIGGER_ROUTINE(timer1_compare_b, 0b101)

#undef ADC_DEFINE_ENABLE_AUTO_TRIGGER_ROUTINE

/*
 * Generate ADC clock prescaler functions.
 *
 * ADCSRA : ADPS2:0 selects ADC clock division factor.
 *          001 = /2,   010 = /4,   011 = /8.
 *          100 = /16,  101 = /32,  110 = /64.
 *          111 = /128.
 */
#define ADC_DEFINE_PRESCALER_ROUTINE(PRESCALER, VALUE)                         \
                                                                             \
    /* Set ADPS2:0: divide CPU clock by selected prescaler. */                 \
    static inline void adc_prescaler_set_##PRESCALER()                        \
    {                                                                        \
        ADCSRA ^= (ADCSRA & 0b111);                                          \
        ADCSRA |= VALUE;                                                     \
    }

ADC_DEFINE_PRESCALER_ROUTINE(2, 1)
ADC_DEFINE_PRESCALER_ROUTINE(4, 2)
ADC_DEFINE_PRESCALER_ROUTINE(8, 3)
ADC_DEFINE_PRESCALER_ROUTINE(16, 4)
ADC_DEFINE_PRESCALER_ROUTINE(32, 5)
ADC_DEFINE_PRESCALER_ROUTINE(64, 6)
ADC_DEFINE_PRESCALER_ROUTINE(128, 7)

#undef ADC_DEFINE_PRESCALER_ROUTINE

/*
 * Generate ADC channel reading functions.
 *
 * ADMUX  : MUX3:0 selects ADC input channel (ADC0-ADC5).
 *          ADLAR = 0 right-adjusts 10-bit result.
 *          ADLAR = 1 left-adjusts result for 8-bit reading.
 * ADCSRA : ADSC = 1 starts conversion; clears when complete.
 * ADC    : combined ADCL/ADCH 10-bit conversion result.
 * ADCH   : upper 8 bits of left-adjusted result.
 */
#define ADC_DEFINE_READ_ROUTINE(PIN)                                          \
                                                                             \
    /* Read ADCn with 10-bit resolution (0-1023). */                          \
    static inline uint16_t adc_read_adc##PIN##_10_bits()                      \
    {                                                                        \
        ADMUX &= ~(1 << ADLAR);                                              \
        ADMUX &= ~(0x0F);                                                    \
        ADMUX |= PIN;                                                        \
        ADCSRA |= (1 << ADSC);                                               \
        while (ADCSRA & (1 << ADSC))                                         \
            ;                                                                \
        return ADC;                                                          \
    }                                                                        \
                                                                             \
    /* Read ADCn with 8-bit resolution (0-255). */                            \
    static inline uint8_t adc_read_adc##PIN##_8_bits()                        \
    {                                                                        \
        ADMUX &= ~(0x3F);                                                    \
        ADMUX |= PIN;                                                        \
        ADMUX |= (1 << ADLAR);                                               \
        ADCSRA |= (1 << ADSC);                                               \
        while (ADCSRA & (1 << ADSC))                                         \
            ;                                                                \
        return ADCH;                                                         \
    }                                                                        \
                                                                             \
    /* Select ADCn, left-adjust result and start 8-bit conversion. */         \
    static inline void adc_read_set_adc##PIN##_8_bits()                       \
    {                                                                        \
        ADMUX &= ~(0x3F);                                                    \
        ADMUX |= PIN;                                                        \
        ADMUX |= (1 << ADLAR);                                               \
        ADCSRA |= (1 << ADSC);                                               \
    }                                                                        \
                                                                             \
    /* Select ADCn, right-adjust result and start 10-bit conversion. */       \
    static inline void adc_read_set_adc##PIN##_10_bits()                      \
    {                                                                        \
        ADMUX &= ~(1 << ADLAR);                                              \
        ADMUX &= ~(0x0F);                                                    \
        ADMUX |= PIN;                                                        \
        ADCSRA |= (1 << ADSC);                                               \
    }

/* Generate ADC input functions for channels ADC0-ADC5. */
ADC_DEFINE_READ_ROUTINE(0)
ADC_DEFINE_READ_ROUTINE(1)
ADC_DEFINE_READ_ROUTINE(2)
ADC_DEFINE_READ_ROUTINE(3)
ADC_DEFINE_READ_ROUTINE(4)
ADC_DEFINE_READ_ROUTINE(5)

#undef ADC_DEFINE_READ_ROUTINE

/*
 * Read internal temperature sensor (raw ADC value).
 *
 * ADMUX : REFS1:0 = 11 selects internal 1.1V reference.
 *         MUX3:0 = 1000 selects temperature sensor.
 *         ADLAR = 0 right-adjusts 10-bit result.
 * ADCSRA: ADSC = 1 starts ADC conversion.
 */
static inline uint16_t adc_read_temperature()
{
    ADMUX &= ~(0x0f) & ~(1 << ADLAR);
    ADMUX = (1 << REFS1) | (1 << REFS0) | (0x08);

    /* Allow internal voltage reference to settle. */
    _delay_ms(5);

    /* Start conversion and wait until complete. */
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC))
        ;

    return ADC;
}

/*
 * Generate ADC voltage reference selection functions.
 *
 * ADMUX : REFS1:0 = 00 -> external AREF pin.
 *                  = 01 -> AVCC.
 *                  = 11 -> internal 1.1V reference.
 */
#define ADC_DEFINE_VOLTAGE_ROUTINE(VOLTAGE_NAME, REF1, REF0)                  \
                                                                             \
    /* Set REFS1:0: select ADC voltage reference. */                           \
    static inline void adc_voltage_set_##VOLTAGE_NAME()                       \
    {                                                                        \
        ADMUX &= ~(1 << REFS1) & ~(1 << REFS0);                               \
        ADMUX |= (REF1 << REFS1) | (REF0 << REFS0);                           \
    }

ADC_DEFINE_VOLTAGE_ROUTINE(aref, 0, 0)
ADC_DEFINE_VOLTAGE_ROUTINE(avcc, 0, 1)
ADC_DEFINE_VOLTAGE_ROUTINE(internal_aref, 1, 1)

#undef ADC_DEFINE_VOLTAGE_ROUTINE

#endif
