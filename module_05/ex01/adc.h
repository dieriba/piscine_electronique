#ifndef ADC_H
#define ADC_H

#include <avr/io.h>
#include <stdint.h>

static inline void adc_enable()
{
    ADCSRA |= (1 << ADEN);
}

#define ADC_DEFINE_PRESCALER_ROUTINE(PRESCALER, VALUE)                                                                                                                                                 \
    static inline void adc_prescaler_set_##PRESCALER()                                                                                                                                                 \
    {                                                                                                                                                                                                  \
        ADCSRA ^= (ADCSRA & 0b111);                                                                                                                                                                    \
        ADCSRA |= VALUE;                                                                                                                                                                               \
    }

ADC_DEFINE_PRESCALER_ROUTINE(2, 1)
ADC_DEFINE_PRESCALER_ROUTINE(4, 2)
ADC_DEFINE_PRESCALER_ROUTINE(8, 3)
ADC_DEFINE_PRESCALER_ROUTINE(16, 4)
ADC_DEFINE_PRESCALER_ROUTINE(32, 5)
ADC_DEFINE_PRESCALER_ROUTINE(64, 6)
ADC_DEFINE_PRESCALER_ROUTINE(128, 7)

#undef ADC_DEFINE_PRESCALER_ROUTINE

#define ADC_DEFINE_READ_ROUTINE(PIN)                                                                                                                                                                   \
    static inline uint16_t adc_read_adc##PIN##_10_bits()                                                                                                                                               \
    {                                                                                                                                                                                                  \
        ADMUX &= ~(1 << ADLAR);                                                                                                                                                                        \
        ADMUX &= ~(0x0F);                                                                                                                                                                              \
        ADMUX |= PIN;                                                                                                                                                                                  \
        ADCSRA |= (1 << ADSC);                                                                                                                                                                         \
        while (ADCSRA & (1 << ADSC))                                                                                                                                                                   \
            ;                                                                                                                                                                                          \
        return ADC;                                                                                                                                                                                    \
    }                                                                                                                                                                                                  \
    static inline uint8_t adc_read_adc##PIN##_8_bits()                                                                                                                                                 \
    {                                                                                                                                                                                                  \
        ADMUX &= ~(0x3F);                                                                                                                                                                              \
        ADMUX |= PIN;                                                                                                                                                                                  \
        ADMUX |= (1 << ADLAR);                                                                                                                                                                         \
        ADCSRA |= (1 << ADSC);                                                                                                                                                                         \
        while (ADCSRA & (1 << ADSC))                                                                                                                                                                   \
            ;                                                                                                                                                                                          \
        return ADCH;                                                                                                                                                                                   \
    }

ADC_DEFINE_READ_ROUTINE(0)
ADC_DEFINE_READ_ROUTINE(1)
ADC_DEFINE_READ_ROUTINE(2)
ADC_DEFINE_READ_ROUTINE(3)
ADC_DEFINE_READ_ROUTINE(4)
ADC_DEFINE_READ_ROUTINE(5)

#undef ADC_DEFINE_READ_ROUTINE

static inline void adc_read_temperature()
{
    ADMUX &= ~(0x0F);
    ADMUX |= (1 << 3);
}

#define ADC_DEFINE_VOLTAGE_ROUTINE(VOLTAGE_NAME, REF1, REF0)                                                                                                                                           \
    static inline void adc_voltage_set_##VOLTAGE_NAME()                                                                                                                                                \
    {                                                                                                                                                                                                  \
        ADMUX &= ~(1 << REFS1) & ~(1 << REFS0);                                                                                                                                                        \
        ADMUX |= (REF1 << REFS1) | (REF0 << REFS0);                                                                                                                                                    \
    }

ADC_DEFINE_VOLTAGE_ROUTINE(aref, 0, 0)
ADC_DEFINE_VOLTAGE_ROUTINE(avcc, 0, 1)
ADC_DEFINE_VOLTAGE_ROUTINE(internal_aref, 1, 1)

#undef ADC_DEFINE_VOLTAGE_ROUTINE

#endif