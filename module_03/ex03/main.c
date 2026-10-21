#include "port.h"
#include "timer.h"
#include "uart.h"
#include "utils.h"

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

#define BUFFER_SIZE 0x100
#define MSG_ENTER_RGB "Enter a hex RGB color (#RRGGBB), e.g., #FF0000: "

#define MSG_ERROR_WRONG_FORMAT                                                                                                                                                                         \
    "ERROR: Invalid RGB color format!\r\n"                                                                                                                                                             \
    "Expected: #RRGGBB (hexadecimal digits: 0-9, A-F).\r\n"
#define FORMAT_MSG "#RRGGBB"
#define BACKSPACE 0x7f
#define ERASE_CHAR_TERM "\b \b"
#define BASE_LEN 16
#define MASK_BYTE 0xff

typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} rgb_color;

void init_rgb()
{
    timer0_set_prescaler_1();
    timer2_set_prescaler_1();
    timer0_set_mode_phase_correct_pwm_top_ff();
    timer2_set_mode_phase_correct_pwm_top_ff();
    timer0_set_compare_output_mode_phase_correct_pwm_clear_on_upcount_set_on_downcount_oc0a();
    timer0_set_compare_output_mode_phase_correct_pwm_clear_on_upcount_set_on_downcount_oc0b();
    timer2_set_compare_output_mode_phase_correct_pwm_clear_on_upcount_set_on_downcount_oc2b();
}

void set_rgb(uint8_t red, uint8_t green, uint8_t blue)
{
    timer0_set_compare_b(red);
    timer0_set_compare_a(green);
    timer2_set_compare_b(blue);
}

void init_gpio()
{
    pd3_output();
    pd5_output();
    pd6_output();
}

uint16_t getline_and_send(char *buffer, uint16_t max_len)
{
    uint16_t i = 0;
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

uint8_t parse_rgb(rgb_color *rgb, char *buffer, uint8_t len_input)
{
    if (len_input != (sizeof(FORMAT_MSG) - 1) || (buffer[0] != FORMAT_MSG[0]))
        return 0;
    const char *end_ptr;
    uint32_t    value = hex_str_to_unsigned(buffer + 1, &end_ptr);

    if (*end_ptr != 0)
        return 0;

    rgb->blue = value & MASK_BYTE;
    value >>= 8;
    rgb->green = value & MASK_BYTE;
    value >>= 8;
    rgb->red = value & MASK_BYTE;
    return 1;
}

int main()
{
    char      user_input[BUFFER_SIZE + 1] = {0};
    rgb_color rgb;
    init_gpio();
    init_rgb();
    uart_init_tx_rx(SENDER | RECEIVER);
    while (1)
    {
        uart_tx_send_str(MSG_ENTER_RGB);
        uint8_t len_input = getline_and_send(user_input, BUFFER_SIZE);
        if (!parse_rgb(&rgb, user_input, len_input))
        {
            uart_tx_send_str(MSG_ERROR_WRONG_FORMAT);
            continue;
        }
        set_rgb(rgb.red, rgb.green, rgb.blue);
    }
}