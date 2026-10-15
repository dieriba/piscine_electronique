#include "adc.h"
#include "uart.h"
#include "utils.h"

#include <stdint.h>
#include <util/delay.h>
#define WAIT_TIME_MS 20
#define BUFFER_SIZE 4

void adc_init()
{
    adc_enable();
    adc_voltage_set_internal_aref();
    adc_prescaler_set_128();
}

int main()
{
    adc_init();
    uart_init_tx_rx(SENDER);
    char buffer[BUFFER_SIZE + 1] = {0};

    while (1)
    {
        uint16_t raw_temp = adc_read_temperature();
        int32_t  celsius  = (((int32_t) raw_temp * 1100 / 1024) - 314) * 100 / 122 + 25;
        itoa(buffer, celsius);
        uart_tx_send_str(buffer);
        uart_tx_send_str("\r\n");
        _delay_ms(WAIT_TIME_MS);
    }
}