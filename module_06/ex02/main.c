#include "twi.h"
#include "uart.h"
#include "utils.h"
#include <math.h>

#include <stdint.h>
#include <util/delay.h>

#define AHT20_ADDR 0x38

#define AHT20_WAIT_TIME_MS_TRIGGER_MEASUREMENT 80
#define AHT20_WAIT_TIME_MS_POWER_BOOT 40
#define AHT20_WAIT_TIME_MS_INIT 10
#define WAIT_TIME_POLL_STATUS 50
#define WAIT_TIME_MEASURE 2000
#define LEN_ARR(arr) (sizeof(arr) / sizeof(*arr))

#define AHT20_MASK_CALIBRATION 0x08
#define AHT20_MASK_BUSY 0x80
#define AHT20_MEASURE_LEN 7

typedef enum AHT20_COMMAND
{
    AHT20_COMMAND_INIT       = 0xBE,
    AHT20_COMMAND_MEASURE    = 0xAC,
    AHT20_COMMAND_GET_STATUS = 0x71
} AHT20_COMMAND;

#define FLOAT_MAX_WIDTH 12

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
    twi_start();
    twi_set_write_address(addr);
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
    hex_read_byte(hex_buffer, c);
    uart_tx_send_str(hex_buffer);
}

void print_measure(uint8_t *data)
{
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

void print_temperature_and_humidity(float temperature, float humidity)
{
    char buffer_temperature[FLOAT_MAX_WIDTH + 1];
    char buffer_humidity[FLOAT_MAX_WIDTH + 1];

    dtostrf(temperature, 0, 1, buffer_temperature);
    dtostrf(humidity, 0, 1, buffer_humidity);

    uart_tx_send_str("Temperature: ");
    uart_tx_send_str(buffer_temperature);
    uart_tx_send_str(".C, ");
    uart_tx_send_str("Humidity: ");
    uart_tx_send_str(buffer_humidity);
    uart_tx_send_byte('%');
    uart_tx_send_str("\r\n");
}

void AHT20_init(void)
{
    _delay_ms(AHT20_WAIT_TIME_MS_POWER_BOOT);
    uint8_t cmd = AHT20_COMMAND_GET_STATUS;
    i2c_write_all_to(AHT20_ADDR, &cmd, sizeof(cmd));
    uint8_t status;
    twi_master_receive_from(AHT20_ADDR, (char *) &status, sizeof(status));
    if ((status & AHT20_MASK_CALIBRATION) == 0)
    {
        uint8_t init_commands[] = {AHT20_COMMAND_INIT, 0x08, 0x00};
        i2c_write_all_to(AHT20_ADDR, init_commands, LEN_ARR(init_commands));
        _delay_ms(AHT20_WAIT_TIME_MS_INIT);
    }
    twi_master_receive_from(AHT20_ADDR, (char *) &status, sizeof(status));
    char hex_buffer[3] = {0};
    hex_read_byte(hex_buffer, status);
    uart_tx_send_str("STATUS: ");
    uart_tx_send_str(hex_buffer);
    uart_tx_send_str("\r\n");
}

void i2c_stop(void)
{
    twi_stop();
}

void AHT20_parse_data(uint8_t *data, float *temperature, float *humidity)
{
    uint32_t temp_data2 = data[3] & 0x0F;
    uint32_t temp_data1 = data[4];
    uint32_t temp_data0 = data[5];

    uint32_t temp_data = (temp_data2 << 16) | (temp_data1 << 8) | temp_data0;
    *temperature       = (float) (((temp_data / pow(2, 20)) * 200) - 50);

    uint32_t hum_data2 = data[1];
    uint32_t hum_data1 = data[2];
    uint32_t hum_data0 = data[3];
    uint32_t hum_data  = (hum_data2 << 16) | (hum_data1 << 8) | hum_data0;
    hum_data >>= 4;
    *humidity = (float) ((hum_data / pow(2, 20)) * 100);
}

void AHT20_measure(float *temperature, float *humidity)
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
        _delay_ms(WAIT_TIME_POLL_STATUS);
    } while (status & AHT20_MASK_BUSY);

    uint8_t data[AHT20_MEASURE_LEN];
    twi_master_receive_from(AHT20_ADDR, (char *) data, AHT20_MEASURE_LEN);
    print_measure(data);
    AHT20_parse_data(data, temperature, humidity);
}

int main()
{
    uart_init_tx_rx(SENDER);
    i2c_init();
    AHT20_init();
    while (1)
    {
        float temperature, humidity;
        AHT20_measure(&temperature, &humidity);
        print_temperature_and_humidity(temperature, humidity);
        _delay_ms(WAIT_TIME_MEASURE);
    }
    i2c_stop();
}