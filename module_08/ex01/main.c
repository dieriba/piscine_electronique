#include "spi.h"
#include <stdint.h>
#include <util/delay.h>

#define LEN_ARR(ARR) (sizeof(ARR) / sizeof(*ARR))
#define APA102_MASK_BRIGHTNESS 0x1F
#define APA102_LED_COUNT 3
#define WAIT_TIME_MS 1000
#define APA102_HEADER 0xE0

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

static apa102_led      leds[APA102_LED_COUNT];
static const rgb_color rgb_channels[] = {
    {0xFF, 0x00, 0x00}, // Red
    {0x00, 0xFF, 0x00}, // Green
    {0x00, 0x00, 0xFF}, // Blue
    {0xFF, 0xFF, 0x00}, // Yellow
    {0x00, 0xFF, 0xFF}, // Cyan
    {0xFF, 0x00, 0xFF}, // Magenta
    {0xFF, 0xFF, 0xFF}  // White
};

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
    apa102_init(0x10);
    uint8_t i = 0;
    while (1)
    {
        i = (i + 1) % LEN_ARR(rgb_channels);
        apa102_update_led_colors(0, &rgb_channels[i]);
        apa102_display();
        _delay_ms(WAIT_TIME_MS);
    }
}