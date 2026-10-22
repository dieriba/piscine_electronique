#include "adc.h"
#include "uart.h"
#include "utils.h"

#include <stdint.h>
#include <util/delay.h>
#define WAIT_TIME_MS 20
#define BUFFER_SIZE 4
#define CALIBRATION_POINT 25
#define ADC_RAW_VALUE 380

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
        int16_t raw_temp = adc_read_temperature();

        int16_t celsius = (((int16_t)raw_temp - ADC_RAW_VALUE) * 4) / 5 + 25; 
		//https://github.com/dm-cdb/Arduino/tree/main/sensor_internal
        signed_number_to_str(buffer, celsius);
        uart_tx_send_str(buffer);
        uart_tx_send_str("\r\n");
        _delay_ms(WAIT_TIME_MS);
    }
}