#include "spi.h"
#include <stdint.h>
#include <util/delay.h>

#define LEN_ARR(ARR) (sizeof(ARR)/sizeof(*ARR))
#define APA102_MASK_BRIGHTNESS 0x1F
#define APA102_LED_COUNT 3
#define WAIT_TIME_MS 1000
#define APA102_HEADER 0xE0

const uint32_t APA102_START_FRAME = 0x00000000;
const uint32_t APA102_END_FRAME = 0xFFFFFFFF;


typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} rgb_color;

typedef  struct __attribute__((packed))  {
    uint8_t settings;
    uint8_t blue;
    uint8_t green;
    uint8_t red;
} led;



static const rgb_color rgb_channels[] = {
    {0xFF, 0x00, 0x00}, // Red
    {0x00, 0xFF, 0x00}, // Green
    {0x00, 0x00, 0xFF}, // Blue
    {0xFF, 0xFF, 0x00}, // Yellow
    {0x00, 0xFF, 0xFF}, // Cyan
    {0xFF, 0x00, 0xFF}, // Magenta
    {0xFF, 0xFF, 0xFF}  // White
};

void init_spi()
{
    spi_master_mode_set();
}

void APA102_send(led *leds, uint8_t len)
{
    spi_master_transmit_data(&APA102_START_FRAME, sizeof(APA102_START_FRAME));
    for (uint8_t i = 0; i < len; i++)
    {
        spi_master_transmit_data(&leds[i], sizeof(*leds));
    }
    spi_master_transmit_data(&APA102_END_FRAME, sizeof(APA102_END_FRAME));
}

void led_init(led *led, uint8_t brightness, uint8_t red, uint8_t green, uint8_t blue)
{
    led->settings = APA102_HEADER | (brightness & APA102_MASK_BRIGHTNESS);
    led->red = red;
    led->green = green;
    led->blue = blue;
}



int main()
{
    init_spi();
    uint8_t i = 0;
    led leds[APA102_LED_COUNT];
    led_init(&leds[1],  0, 0, 0, 0);
    led_init(&leds[2], 0, 0, 0, 0);
    while (1)
    {
        i = i % LEN_ARR(rgb_channels);
        led_init(&leds[0], 0xa, rgb_channels[i].red, rgb_channels[i].green, rgb_channels[i].blue);
        APA102_send(leds, APA102_LED_COUNT);
        _delay_ms(WAIT_TIME_MS);
        i++;
    }
}