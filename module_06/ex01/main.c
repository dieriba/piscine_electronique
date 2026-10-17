#include "twi.h"
#include "uart.h"
#include "utils.h"

#include <stdint.h>
#include <util/delay.h>

#define AHT20_ADDR 0x38

#define AHT20_WAIT_TIME_MS_TRIGGER_MEASUREMENT 80
#define AHT20_WAIT_TIME_MS_POWER_BOOT 40
#define AHT20_WAIT_TIME_MS_INIT 10
#define WAIT_TIME_MEASURE 1000

#define AHT20_MEASURE_LEN 7

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

void i2c_write(unsigned char data)
{
    twi_write_data(data);
}

void i2c_write_all(uint8_t *data, uint8_t len)
{
    twi_writen((char *) data, len);
}

void i2c_read(void)
{
}

void print_hex_value(char c)
{
    char hex_buffer[3];
    hex_read(hex_buffer, c);
    uart_tx_send_str(hex_buffer);
}

void print_measure()
{
    uint8_t data[AHT20_MEASURE_LEN];
    twi_readn((char *) data, AHT20_MEASURE_LEN);
    for (uint8_t i = 0; i < AHT20_MEASURE_LEN; i++)
    {
        print_hex_value(data[i]);
        if (i != AHT20_MEASURE_LEN - 1)
        {
            uart_tx_send_str(" ");
        }
    }

    uart_tx_send_str("\r\n");
}

void AHT20_init(void)
{
    _delay_ms(AHT20_WAIT_TIME_MS_POWER_BOOT);
    twi_set_read_address(AHT20_COMMAND_GET_STATUS);
    uint8_t status = twi_read_byte_nack(NULL);
    if ((status >> 3 & 0x01) == 0) // if CAL = 0
    {
        uint8_t init_commands[3] = {0xBE, 0x08, 0x00};
        twi_start();
        twi_set_write_address(AHT20_ADDR);
        i2c_write_all(init_commands, sizeof(init_commands) / sizeof(uint8_t));
        _delay_ms(AHT20_WAIT_TIME_MS_INIT);
    }
}

void AHT20_measure(void)
{
    uint8_t measure_command[3] = {0xAC, 0x33, 0x00};
    HAL_I2C_Master_Transmit(&hi2c1, AHT20_ADDRESS, measure_command, 3, 1000);
    _delay_ms(AHT20_WAIT_TIME_MS_TRIGGER_MEASUREMENT);

    uint8_t status;
    do
    {
        HAL_I2C_Mem_Read(&hi2c1, AHT20_ADDRESS, 0x71, 1, &status, 1, 1000);
        _delay_ms(100);
    } while ((status >> 7 & 0x01) == 1);

    print_measure();
}

int main()
{
    uart_init_tx_rx(SENDER);
    i2c_init();
    i2c_start();
    AHT20_init();
    while (1)
    {
        AHT20_measure();
        _delay_ms(WAIT_TIME_MEASURE);
    }
}