#include "twi.h"
#include "uart.h"
#include "utils.h"

#include <stdint.h>
#include <stdlib.h>
#include <util/delay.h>

#define AHT20_ADDR 0x38

#define AHT20_WAIT_TIME_MS_TRIGGER_MEASUREMENT 80
#define AHT20_WAIT_TIME_MS_POWER_BOOT 40
#define AHT20_WAIT_TIME_MS_INIT 10
#define WAIT_TIME_POLL_STATUS 50
#define WAIT_TIME_MEASURE 2000
#define LEN_ARR(arr) (sizeof(arr) / sizeof(*arr))
#define COUNT 3.0f
#define AHT20_MASK_CALIBRATION 0x08
#define AHT20_MASK_BUSY 0x80
#define AHT20_MEASURE_LEN 7
#define DIVISOR (double) 1048576

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

void aht20_write_all(uint8_t *data, uint8_t len)
{
    twi_start();
    twi_set_write_address(AHT20_ADDR);
    for (uint8_t i = 0; i < len; i++)
    {
        i2c_write(data[i]);
    }
}

void print_hex_value(char c)
{
    char hex_buffer[3] = {0};
    hex_read_byte(hex_buffer, c);
    uart_tx_send_str(hex_buffer);
}

void char_replace_by(char *buffer, char to_find, char new)
{
    for (uint16_t i = 0; buffer[i]; i++)
    {
        if (buffer[i] == to_find)
            buffer[i] = new;
    }
}

void print_temperature_and_humidity(float temperature, float humidity)
{
    char buffer_temperature[FLOAT_MAX_WIDTH + 1];
    char buffer_humidity[FLOAT_MAX_WIDTH + 1];

    dtostrf(temperature, 0, 2, buffer_temperature);
    char_replace_by(buffer_temperature, '.', ',');
    dtostrf(humidity, 0, 3, buffer_humidity);
    char_replace_by(buffer_humidity, '.', ',');

    uart_tx_send_str("Temperature: ");
    uart_tx_send_str(buffer_temperature);
    uart_tx_send_str("C, ");
    uart_tx_send_str("Humidity: ");
    uart_tx_send_str(buffer_humidity);
    uart_tx_send_str_with_nl("%");
}

void aht20_init(void)
{
    _delay_ms(AHT20_WAIT_TIME_MS_POWER_BOOT);
    uint8_t cmd = AHT20_COMMAND_GET_STATUS;
    aht20_write_all(&cmd, sizeof(cmd));
    uint8_t status;
    twi_master_receive_from(AHT20_ADDR, (char *) &status, sizeof(status));
    if ((status & AHT20_MASK_CALIBRATION) == 0)
    {
        uint8_t init_commands[] = {AHT20_COMMAND_INIT, 0x08, 0x00};
        aht20_write_all(init_commands, LEN_ARR(init_commands));
        _delay_ms(AHT20_WAIT_TIME_MS_INIT);
    }
    twi_master_receive_from(AHT20_ADDR, (char *) &status, sizeof(status));
}

void i2c_stop(void)
{
    twi_stop();
}

void aht20_parse_data(uint8_t *data, float *temperature, float *humidity)
{
    uint32_t temp_data2 = data[3] & 0x0F;
    uint32_t temp_data1 = data[4];
    uint32_t temp_data0 = data[5];

    uint32_t temp_data = (temp_data2 << 16) | (temp_data1 << 8) | temp_data0;
    *temperature       = (float) (((temp_data / DIVISOR) * 200) - 50);

    uint32_t hum_data2 = data[1];
    uint32_t hum_data1 = data[2];
    uint32_t hum_data0 = data[3];
    uint32_t hum_data  = (hum_data2 << 16) | (hum_data1 << 8) | hum_data0;
    hum_data >>= 4;
    *humidity = (float) ((hum_data / DIVISOR) * 100);
}

void aht20_measure(float *temperature, float *humidity)
{
    uint8_t measure_command[] = {AHT20_COMMAND_MEASURE, 0x33, 0x00};
    aht20_write_all(measure_command, LEN_ARR(measure_command));
    _delay_ms(AHT20_WAIT_TIME_MS_TRIGGER_MEASUREMENT);

    uint8_t status;
    do
    {
        twi_master_receive_from(AHT20_ADDR, (char *) &status, 1);
        _delay_ms(WAIT_TIME_POLL_STATUS);
    } while (status & AHT20_MASK_BUSY);

    uint8_t data[AHT20_MEASURE_LEN];
    twi_master_receive_from(AHT20_ADDR, (char *) data, AHT20_MEASURE_LEN);
    aht20_parse_data(data, temperature, humidity);
}

void i2c_read(void)
{
    float average_temperature = 0, average_humidity = 0;
    float temperature, humidity;

    for (uint8_t i = 0; i < COUNT; i++)
    {
        aht20_measure(&temperature, &humidity);
        average_temperature += temperature;
        average_humidity += humidity;
    }

    average_temperature = average_temperature / COUNT;
    average_humidity    = average_humidity / COUNT;
    print_temperature_and_humidity(average_temperature, average_humidity);
}

int main()
{
    uart_init_tx_rx(SENDER);
    i2c_init();
    i2c_start();

    uint8_t data;
    twi_read_byte_nack(&data);
    i2c_stop();

    aht20_init();
    while (1)
    {
        i2c_read();
        _delay_ms(WAIT_TIME_MEASURE);
    }
}