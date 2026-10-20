#include "adc.h"
#include "interrupt.h"
#include "port.h"
#include "spi.h"
#include "timer.h"

#include <stdint.h>
#include <util/delay.h>

#define LEN_ARR(ARR) (sizeof(ARR) / sizeof(*ARR))
#define APA102_MASK_BRIGHTNESS 0x1F
#define APA102_LED_COUNT 3
#define APA102_HEADER 0xE0
#define TIMER1_FREQ 15625
#define MS 0.064
#define TARGET_WAIT_TIME_MS 20
#define B ((TARGET_WAIT_TIME_MS / MS) + 1)

typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} rgb_color;

typedef struct __attribute__((packed))
{
    uint8_t settings;
    uint8_t blue;
    uint8_t green;
    uint8_t red;
} apa102_led;

uint8_t color_value;

static apa102_led leds[APA102_LED_COUNT];

static const uint32_t APA102_START_FRAME = 0x00000000;
static const uint32_t APA102_END_FRAME   = 0xFFFFFFFF;

void init_spi()
{
    spi_master_mode_set();
}

void apa102_display()
{
    spi_master_transmit_data(&APA102_START_FRAME, sizeof(APA102_START_FRAME));
    spi_master_transmit_data(leds, sizeof(leds));
    spi_master_transmit_data(&APA102_END_FRAME, sizeof(APA102_END_FRAME));
}

void apa102_init(uint8_t brightness)
{
    for (uint8_t i = 0; i < APA102_LED_COUNT; i++)
    {
        leds[i].settings = APA102_HEADER | (brightness & APA102_MASK_BRIGHTNESS);
        leds[i].red      = 0;
        leds[i].green    = 0;
        leds[i].blue     = 0;
    }
}

void apa102_clear_color()
{
    for (uint8_t i = 0; i < APA102_LED_COUNT; i++)
    {
        leds[i].red   = 0;
        leds[i].green = 0;
        leds[i].blue  = 0;
    }
}

void apa102_update_led_colors(uint8_t index, const rgb_color *color)
{
    leds[index].red   = color->red;
    leds[index].green = color->green;
    leds[index].blue  = color->blue;
}

void init_gpio()
{
    pd2_input();
    pd2_pull_up_enable();
    pd4_input();
    pd4_pull_up_enable();
}

void adc_init()
{
    adc_enable();
    adc_voltage_set_avcc();
    adc_prescaler_set_128();
    adc_enable_interrupt();
    adc_enable_auto_trigger_src_timer1_compare_b();
    adc_read_set_adc0_8_bits();
}

void init_timer1()
{
    timer1_set_compare_b(B);
    timer1_set_compare_a(B);
    timer1_set_mode_ctc();
    timer1_set_prescaler_1024();
}

SIGNAL(ADC_vect)
{
    color_value = ADCH;

    TIFR1 = (1 << OCF1B); // CLEAR OUTPUT COMPARE FLAG B
}

int main()
{
    init_spi();
    init_gpio();
    adc_init();
    apa102_init(0x10);
    interrupt_global_enable();
    while (1)
    {
    }
}