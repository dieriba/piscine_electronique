#include "adc.h"
#include "hex.h"
#include "uart.h"

#include <stdint.h>
#include <util/delay.h>
#define WAIT_TIME_MS 20
#define HEX_BUFFER_SIZE 2

void adc_init()
{
    adc_enable();
    adc_voltage_set_avcc();
    adc_prescaler_set_128();
}

int main()
{
    adc_init();
    uart_init_tx_rx(SENDER);
    char hex_buffer[HEX_BUFFER_SIZE + 1] = {0};
    while (1)
    {
        uint8_t rv1 = adc_read_adc0_8_bits();
        hex_read(hex_buffer, rv1);
        uart_tx_send_str(hex_buffer);
        uart_tx_send_str("\r\n");
        _delay_ms(WAIT_TIME_MS);
    }
}