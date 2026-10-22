#include "adc.h"
#include "uart.h"
#include "utils.h"
#include <stdint.h>
#include <util/delay.h>
#define WAIT_TIME_MS 20
#define NB_TO_DISPLAY 3
#define SUFFIX ", "
#define BUFFER_SIZE ((NB_TO_DISPLAY * 4) + ((NB_TO_DISPLAY - 1) * (sizeof(SUFFIX) - 1)))


void adc_init()
{
    adc_enable();
    adc_voltage_set_avcc();
    adc_prescaler_set_128();
}

void buffer_update(char *buffer, uint16_t *value)
{
    uint8_t offset = 0;
    for (uint8_t i = 0; i < NB_TO_DISPLAY; i++)
    {
        uint8_t len = unsigned_number_to_str(buffer + offset, value[i]);
        offset += len;
        if (i != (NB_TO_DISPLAY - 1))
        {
            buffer[offset]     = ',';
            buffer[offset + 1] = ' ';
        }
        offset += 2;
    }
}

int main()
{
    char     buffer[BUFFER_SIZE + 1] = {0};
    uint16_t value[NB_TO_DISPLAY];
    adc_init();

    uart_init_tx_rx(SENDER);
    while (1)
    {
        uint16_t rv1 = adc_read_adc0_10_bits();
        uint16_t r14 = adc_read_adc1_10_bits();
        uint16_t r20 = adc_read_adc2_10_bits();
        value[0]     = rv1;
        value[1]     = r14;
        value[2]     = r20;
        buffer_update(buffer, value);
        uart_tx_send_str(buffer);
        uart_tx_send_str("\r\n");
        _delay_ms(WAIT_TIME_MS);
    }
}