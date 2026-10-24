#include "spi.h"

#include <stdint.h>
#include <util/delay.h>

#define LEN_ARR(ARR) (sizeof(ARR) / sizeof(*ARR))
#define APA102_MASK_BRIGHTNESS 0x1F
#define APA102_LED_COUNT 3
#define WAIT_TIME_MS 1000
#define DEFAULT_BRIGHTNESS 0x10

#define APA102_HEADER 0xE0
#define DURATION_CYCLE_MS 1000
#define DURATION_LED_ON_MS 250
#define DURATION_BEFORE_RESTART_CYCLE_MS (DURATION_CYCLE_MS - (DURATION_LED_ON_MS * APA102_LED_COUNT))

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

static apa102_led leds[APA102_LED_COUNT];

static const rgb_color colors[APA102_LED_COUNT] = {{255, 0, 0}, {0, 255, 0}, {0, 0, 255}};

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

int main()
{
    init_spi();
    apa102_init(DEFAULT_BRIGHTNESS);
    while (1)
    {
        for (uint8_t i = 0; i < APA102_LED_COUNT; i++)
        {
            apa102_update_led_colors(i, &colors[i]);
            apa102_display();
            _delay_ms(DURATION_LED_ON_MS);
            apa102_clear_color();
        }
        apa102_display();
        _delay_ms(DURATION_BEFORE_RESTART_CYCLE_MS);
    }
}