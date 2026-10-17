#include "twi.h"
#include "uart.h"

#include <util/delay.h>

#define AHT20_ADDR 0x38

#define AHT20_TRIGGER_MEASUREMENT_TIME_MS 80

typedef enum AHT20_COMMAND
{
    AHT20_COMMAND_INIT       = 0xBE,
    AHT20_COMMAND_MEASURE    = 0xAC,
    AHT20_COMMAND_GET_STATUS = 0x71
} AHT20_COMMAND;

void i2c_init(void)
{
    twi_set_prescaler1();
    twi_set_bitrate(72);
}

void i2c_start(void)
{
    twi_start();
    twi_set_read_address(AHT20_ADDR);
}

void i2c_stop(void)
{
    twi_stop();
}

int main()
{
    uart_init_tx_rx(SENDER);
    i2c_init();
    i2c_start();
    i2c_stop();
    while (1)
    {
    }
}