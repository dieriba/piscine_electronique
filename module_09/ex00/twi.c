#include "twi.h"


#include <avr/io.h>
#include <stdint.h>

#ifdef DEBUG
#include "utils.h"
#include "uart.h"
#endif

#define MASTER_RECEIVER_MODE 0x1
#define MASTER_TRANSMITTER_MODE 0x0
#define HEX_BUFFER_SIZE 2
char hex_buffer[HEX_BUFFER_SIZE + 1];

char *twi_get_status_msg(TWI_STATUS status)
{
    switch (status)
    {
    case TWI_STATUS_START:
        return "START transmitted";
    case TWI_STATUS_REPEATED_START:
        return "Repeated START transmitted";
    case TWI_STATUS_MT_SLA_ACK:
        return "SLA+W transmitted, ACK received";
    case TWI_STATUS_MT_SLA_NACK:
        return "SLA+W transmitted, NACK received";
    case TWI_STATUS_MT_DATA_ACK:
        return "Data transmitted, ACK received";
    case TWI_STATUS_MT_DATA_NACK:
        return "Data transmitted, NACK received";
    case TWI_STATUS_ARBITRATION_LOST:
        return "Arbitration lost";
    case TWI_STATUS_MR_SLA_ACK:
        return "SLA+R transmitted, ACK received";
    case TWI_STATUS_MR_SLA_NACK:
        return "SLA+R transmitted, NACK received";
    case TWI_STATUS_MR_DATA_ACK:
        return "Data received, ACK returned";
    case TWI_STATUS_MR_DATA_NACK:
        return "Data received, NACK returned";
    case TWI_STATUS_BUS_ERROR:
        return "Illegal START or STOP condition";
    case TWI_STATUS_NO_INFO:
        return "No relevant state information";
    default:
        return "Unknown TWI status";
    }
}

uint8_t twi_wait_for_transmission(void)
{
    while (!(TWCR & (1 << TWINT)))
        ;
    TWI_STATUS status = (TWSR & 0xF8);
#ifdef DEBUG
    uart_tx_send_str("Status: 0x");
    hex_read_byte(hex_buffer, status);
    uart_tx_send_str(hex_buffer);
    uart_tx_send_str(" - ");
    uart_tx_send_str(twi_get_status_msg(status));
    uart_tx_send_str("\r\n");
#endif
    return status;
}

void twi_init(uint32_t frequency)
{
    TWBR = ((F_CPU / frequency) - 16) / 2;
    TWCR = (1 << TWEN);
}

uint8_t twi_start(void)
{
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    return twi_wait_for_transmission();
}

void twi_stop()
{
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

uint8_t twi_read_byte_nack(uint8_t *data)
{
    TWCR           = (1 << TWINT) | (1 << TWEN);
    uint8_t status = twi_wait_for_transmission();
    *data          = TWDR;
    return status;
}

uint8_t twi_read_byte_ack(uint8_t *data)
{
    TWCR           = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    uint8_t status = twi_wait_for_transmission();
    *data          = TWDR;

    return status;
}

uint8_t twi_master_receive_from(uint8_t addr, char *buffer, uint8_t len)
{
    uint8_t status = twi_start();
    if ((status != TWI_STATUS_START) && (status != TWI_STATUS_REPEATED_START))
        return status;
    if ((status = twi_set_read_address(addr)) != TWI_STATUS_MR_SLA_ACK)
        return status;

    uint8_t i = 0;
    for (; i < len - 1; i++)
    {
        uint8_t status = twi_read_byte_ack((uint8_t *) buffer + i);
        if (status != TWI_STATUS_MR_DATA_ACK)
            return status;
    }
    return twi_read_byte_nack((uint8_t *) buffer + i);
}

uint8_t twi_master_transmit_to(uint8_t addr, char *buffer, uint8_t len)
{
    uint8_t status = twi_start();
    if ((status != TWI_STATUS_START) && (status != TWI_STATUS_REPEATED_START))
        return status;
    if ((status = twi_set_write_address(addr)) != TWI_STATUS_MT_SLA_ACK)
        return status;

    uint8_t i = 0;
    for (; i < len - 1; i++)
    {
        uint8_t status = twi_write_data(buffer[i]);
        if (status != TWI_STATUS_MT_DATA_NACK)
            return status;
    }
    return twi_write_data(buffer[i]);
}

uint8_t twi_set_read_address(uint8_t addr)
{
    TWDR = (addr << 1) | MASTER_RECEIVER_MODE;
    TWCR = (1 << TWINT) | (1 << TWEN);
    return twi_wait_for_transmission();
}

uint8_t twi_write_data(uint8_t byte)
{
    TWDR = byte;
    TWCR = (1 << TWINT) | (1 << TWEN);
    return twi_wait_for_transmission();
}

uint8_t twi_set_write_address(uint8_t addr)
{
    TWDR = (addr << 1) | MASTER_TRANSMITTER_MODE;
    TWCR = (1 << TWINT) | (1 << TWEN);
    return twi_wait_for_transmission();
}
