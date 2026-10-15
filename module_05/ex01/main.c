#include "adc.h"
#include "hex.h"
#include "uart.h"

#include <stdint.h>
#include <util/delay.h>
#define WAIT_TIME_MS 20
#define NB_HEX_TO_DISPLAY 3
#define SUFFIX ", "
#define HEX_BUFFER_SIZE ((NB_HEX_TO_DISPLAY * 2) + ((NB_HEX_TO_DISPLAY - 1) * (sizeof(SUFFIX) - 1)))

void adc_init()
{
    adc_enable();
    adc_voltage_set_avcc();
    adc_prescaler_set_128();
}

void hex_buffer_init(char *buffer)
{
    for (uint8_t i = 0; i < NB_HEX_TO_DISPLAY - 1; i++)
    {
        uint8_t idx     = i * 4;
        buffer[idx + 2] = ',';
        buffer[idx + 3] = ' ';
    }
}

void hex_buffer_update(char *buffer, uint8_t *hex_value)
{
    for (uint8_t i = 0; i < NB_HEX_TO_DISPLAY; i++)
    {
        hex_read(buffer + (i * 4), hex_value[i]);
    }
}

int main()
{
    char    hex_buffer[HEX_BUFFER_SIZE + 1] = {0};
    uint8_t hex_value[NB_HEX_TO_DISPLAY];
    hex_buffer_init(hex_buffer);
    adc_init();

    uart_init_tx_rx(SENDER);
    while (1)
    {
        uint8_t rv1  = adc_read_adc0_8_bits();
        uint8_t r14  = adc_read_adc1_8_bits();
        uint8_t r20  = adc_read_adc2_8_bits();
        hex_value[0] = rv1;
        hex_value[1] = r14;
        hex_value[2] = r20;
        hex_buffer_update(hex_buffer, hex_value);
        uart_tx_send_str(hex_buffer);
        uart_tx_send_str("\r\n");
        _delay_ms(WAIT_TIME_MS);
    }
}