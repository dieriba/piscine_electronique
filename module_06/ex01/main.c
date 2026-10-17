#include "twi.h"
#include "uart.h"
#include "utils.h"

#include <stdint.h>
#include <util/delay.h>

#define AHT20_ADDR 0x38

#define AHT20_WAIT_TIME_MS_TRIGGER_MEASUREMENT 80
#define AHT20_WAIT_TIME_MS_POWER_BOOT 40
#define AHT20_WAIT_TIME_MS_INIT 10
#define WAIT_TIME_MEASURE 2000
#define LEN_ARR(arr) (sizeof(arr) / sizeof(*arr))

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

void i2c_write_all_to(uint8_t addr, uint8_t *data, uint8_t len)
{
    uart_tx_send_str("i2c_write_all_to 0\r\n");
    twi_start();
    uart_tx_send_str("i2c_write_all_to 1\r\n");
    twi_set_write_address(addr);
    uart_tx_send_str("i2c_write_all_to 2\r\n");
    for (uint8_t i = 0; i < len; i++)
    {
        i2c_write(data[i]);
    }
}

void i2c_read(void)
{
}

void print_hex_value(char c)
{
    char hex_buffer[3] = {0};
    hex_read(hex_buffer, c);
    uart_tx_send_str(hex_buffer);
}

void print_measure()
{
    uint8_t data[AHT20_MEASURE_LEN];
    twi_master_receive_from(AHT20_ADDR, (char *) data, AHT20_MEASURE_LEN);
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

    uart_tx_send_str("0\r\n");
    uint8_t cmd = AHT20_COMMAND_GET_STATUS;
    i2c_write_all_to(AHT20_ADDR, &cmd, sizeof(cmd));
    uart_tx_send_str("1\r\n");
    uint8_t status;
    twi_master_receive_from(AHT20_ADDR, (char *) &status, sizeof(status));
    if ((status >> 3 & 0x01) == 0) // if CAL = 0
    {
        uint8_t init_commands[] = {AHT20_COMMAND_INIT, 0x08, 0x00};
        i2c_write_all_to(AHT20_ADDR, init_commands, LEN_ARR(init_commands));
        _delay_ms(AHT20_WAIT_TIME_MS_INIT);
    }
}

void i2c_stop(void)
{
    twi_stop();
}

void AHT20_measure(void)
{
    uint8_t measure_command[] = {AHT20_COMMAND_MEASURE, 0x33, 0x00};
    i2c_write_all_to(AHT20_ADDR, measure_command, LEN_ARR(measure_command));
    _delay_ms(AHT20_WAIT_TIME_MS_TRIGGER_MEASUREMENT);

    uint8_t status;
    do
    {
        uint8_t cmd = AHT20_COMMAND_GET_STATUS;
        i2c_write_all_to(AHT20_ADDR, &cmd, sizeof(cmd));

        twi_master_receive_from(AHT20_ADDR, (char *) &status, sizeof(status));
        _delay_ms(100);
    } while ((status >> 7 & 0x01) == 1);

    print_measure();
}

int main()
{
    uart_init_tx_rx(SENDER);
    uart_tx_send_str("THERE 1\r\n");
    i2c_init();
    uart_tx_send_str("THERE 2\r\n");
    //i2c_stop();
    uart_tx_send_str("THERE 3\r\n");
    AHT20_init();
    uart_tx_send_str("THERE 4\r\n");
    while (1)
    {
        AHT20_measure();
        _delay_ms(WAIT_TIME_MEASURE);
    }
    i2c_stop();
}