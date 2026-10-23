#include "spi.h"
#include "uart.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>

#define LEN_ARR(ARR) (sizeof(ARR) / sizeof(*ARR))
#define APA102_MASK_BRIGHTNESS 0x1F
#define APA102_LED_COUNT 3
#define APA102_HEADER 0xE0
#define BUFFER_SIZE 0x100
#define MSG_ENTER_RGB "Enter a hex RGB color (#RRGGBBDX), e.g., #FF0000D7: "
#define FULLRAINBOW "#FULLRAINBOW"
#define MSG_ERROR_WRONG_FORMAT                                                                                                                                                                         \
    "ERROR: Invalid RGB color format!\r\n"                                                                                                                                                             \
    "Expected: #RRGGBBDX (hexadecimal digits: 0-9, A-F).\r\n"
#define FORMAT_MSG "#RRGGBBDX"
#define BACKSPACE 0x7f
#define ERASE_CHAR_TERM "\b \b"
#define BASE_LEN 16
#define MASK_BYTE 0xff
#define POS_FIRST_LED 6
#define DEFAULT_BRIGHTNESS 0x10

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

uint8_t getline_and_send(char *buffer, uint16_t max_len)
{
    uint8_t i = 0;
    while (1)
    {
        char c = uart_rx_blocking_get_byte();
        if ((c != '\r' && c != BACKSPACE) && i >= max_len)
        {
            while (i > 0)
            {
                uart_tx_send_str(ERASE_CHAR_TERM);
                i--;
            }
        }
        buffer[i] = c;
        switch (c)
        {
        case '\r':
            uart_tx_send_str("\r\n");
            goto end;
        case BACKSPACE:
            if (i == 0)
                continue;
            i -= 1;
            uart_tx_send_str(ERASE_CHAR_TERM);
            continue;
        default:
            uart_tx_send_byte(c);
            break;
        }
        i++;
    }
end:
    buffer[i] = 0;
    return i;
}

uint8_t parse_input(uint8_t *led, rgb_color *rgb, char *buffer, uint8_t len_input)
{
    if (len_input != (sizeof(FORMAT_MSG) - 1) || (buffer[0] != FORMAT_MSG[0]))
        return 0;

    *led = (buffer[len_input - 1] - '0') - POS_FIRST_LED;
    if (buffer[len_input - 2] != FORMAT_MSG[len_input - 2] || !(*led >= 6 && *led <= 8))
        return 0;
    buffer[len_input - 2] = 0;
    char    *end_ptr;
    uint32_t value = strtol(buffer + 1, &end_ptr, BASE_LEN);

    if (*end_ptr != 0)
        return 0;
    uart_tx_send_str("BUFFER: ");
    uart_tx_send_str_with_nl(buffer);
    rgb->blue = value & MASK_BYTE;
    value >>= 8;
    rgb->green = value & MASK_BYTE;
    value >>= 8;
    rgb->red = value & MASK_BYTE;
    return 1;
}

rgb_color wheel(uint8_t pos)
{
    pos = 255 - pos;

    if (pos < 85)
    {
        return (rgb_color){255 - pos * 3, 0, pos * 3};
    }
    else if (pos < 170)
    {
        pos -= 85;
        return (rgb_color){0, pos * 3, 255 - pos * 3};
    }
    else
    {
        pos -= 170;
        return (rgb_color){pos * 3, 255 - pos * 3, 0};
    }
}

void full_rainbow(uint8_t pos)
{
    for (uint8_t i = 0; i < APA102_LED_COUNT; i++)
    {
        rgb_color color = wheel(pos + i * 85);
        apa102_update_led_colors(i, &color);
    }
}

int main()
{
    char      user_input[BUFFER_SIZE + 1] = {0};
    rgb_color rgb;
    uint8_t   led;
    uint8_t   pos = 0;
    init_spi();
    apa102_init(DEFAULT_BRIGHTNESS);
    uart_init_tx_rx(SENDER | RECEIVER);
    while (1)
    {
        uart_tx_send_str(MSG_ENTER_RGB);
        uint8_t len_input = getline_and_send(user_input, BUFFER_SIZE);
        if (strcmp(user_input, FULLRAINBOW) != 0)
        {
            if (!parse_input(&led, &rgb, user_input, len_input))
            {
                uart_tx_send_str(MSG_ERROR_WRONG_FORMAT);
                continue;
            }
            apa102_update_led_colors(led, &rgb);
        }
        else
            full_rainbow(pos++);
        apa102_display();
    }
}