
#include "port.h"
#include "uart.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include <string.h>
#include <util/delay.h>
#define UART_BAUDRATE 9600
#define UBRR_VAL ((F_CPU / (16UL * UART_BAUDRATE)) - 1)

#define USERNAME "dieriba"
#define PASSWORD "password"
#define BACKSPACE 0x7f
#define ERASE_CHAR_TERM "\b \b"
#define MAX_LEN_USER_INPUT 256
#define WAIT_TIME_MS 600
#define DRAMATIC_EFFECT_WAIT_TIME_MS 100
#define MSG_WELCOME "Hello, "

#define ERROR_MSG_BAD_USERNAME "Bad combinaison username/password\r\n"

void uart_tx(uint8_t byte)
{
}

char uart_rx()
{
    while (uart_rx_buffer_is_empty())
        ;
    uint8_t byte = uart_rx_get_byte();
    return byte;
}

char char_self(char c)
{
    return c;
}

char char_hide(char c)
{
    (void) c;
    return '*';
}

uint8_t getline_and_send(char *buffer, uint16_t max_len, char (*transform)(char))
{
    uint8_t i = 0;
    while (1)
    {
        char c = uart_rx();
        if (c != '\r' && i >= max_len)
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

void init_gpio()
{
    pb0_output();
    pb1_output();
    pb2_output();
    pb4_output();
}

void led_reset()
{
    pb0_low();
    pb1_low();
    pb2_low();
    pb4_low();
}

void led_dramatic_effect()
{
    for (uint8_t i = 0; i < 20; i++)
    {
        pb0_toggle();
        pb1_toggle();
        pb2_toggle();
        pb4_toggle();
        _delay_ms(DRAMATIC_EFFECT_WAIT_TIME_MS);
    }
}

void led_toggle()
{
    while (1)
    {
        pb0_toggle();
        pb1_toggle();
        pb2_toggle();
        pb4_toggle();
        _delay_ms(WAIT_TIME_MS);
    }
}

int main()
{
    char user_input[MAX_LEN_USER_INPUT] = {0};
login:
    uart_init_tx_rx(UBRR_VAL + 1, SENDER | RECEIVER);
    uart_tx_send_str("Enter your login:\r\n");
    uart_tx_send_str("\tusername: ");
    uint8_t len_input      = getline_and_send(user_input, MAX_LEN_USER_INPUT, char_self);
    uint8_t valid_username = (len_input == sizeof(USERNAME) - 1) && memcmp(USERNAME, user_input, len_input) == 0;
    uart_tx_send_str("\tpassword: ");
    len_input              = getline_and_send(user_input, MAX_LEN_USER_INPUT, char_hide);
    uint8_t valid_paswword = (len_input == sizeof(PASSWORD) - 1) && memcmp(PASSWORD, user_input, len_input) == 0;

    if (valid_username && valid_paswword)
    {
        led_reset();
        uart_tx_send_str(MSG_WELCOME USERNAME "!\r\n");
        uart_tx_send_str("Shall we play a game?\r\n");
        led_toggle();
    }
    else
    {
        pb1_high();
        uart_tx_send_str(ERROR_MSG_BAD_USERNAME);
        led_dramatic_effect();
        led_reset();
        goto login;
    }

    while (1)
    {
    }
}