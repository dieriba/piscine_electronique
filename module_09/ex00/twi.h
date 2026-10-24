#ifndef TWI_H
#define TWI_H
#include <avr/io.h>
#include <stdint.h>

#define TWI_DEFINE_PRESCALER_ROUTINE(PRESCALER, VALUE)                                                                                                                                                 \
    static inline void twi_set_prescaler##PRESCALER()                                                                                                                                                  \
    {                                                                                                                                                                                                  \
        TWSR &= ~(1 << TWPS1) & ~(1 << TWPS0);                                                                                                                                                         \
        TWSR |= VALUE;                                                                                                                                                                                 \
    }

TWI_DEFINE_PRESCALER_ROUTINE(1, 0)
TWI_DEFINE_PRESCALER_ROUTINE(4, 1)
TWI_DEFINE_PRESCALER_ROUTINE(16, 2)
TWI_DEFINE_PRESCALER_ROUTINE(64, 3)

#undef TWI_DEFINE_PRESCALER_ROUTINE

static inline void twi_set_bitrate(uint8_t bitrate)
{
    TWBR = bitrate;
}

typedef enum TWI_STATUS
{
    TWI_STATUS_BUS_ERROR        = 0x00,
    TWI_STATUS_START            = 0x08,
    TWI_STATUS_REPEATED_START   = 0x10,
    TWI_STATUS_MT_SLA_ACK       = 0x18,
    TWI_STATUS_MT_SLA_NACK      = 0x20,
    TWI_STATUS_MT_DATA_ACK      = 0x28,
    TWI_STATUS_MT_DATA_NACK     = 0x30,
    TWI_STATUS_ARBITRATION_LOST = 0x38,
    TWI_STATUS_MR_SLA_ACK       = 0x40,
    TWI_STATUS_MR_SLA_NACK      = 0x48,
    TWI_STATUS_MR_DATA_ACK      = 0x50,
    TWI_STATUS_MR_DATA_NACK     = 0x58,
    TWI_STATUS_NO_INFO          = 0xF8
} TWI_STATUS;

uint8_t twi_start(void);
void    twi_stop();
uint8_t twi_wait_for_transmission(void);
char   *twi_get_status_msg(TWI_STATUS status);
uint8_t twi_read_byte_ack(uint8_t *data);
uint8_t twi_read_byte_nack(uint8_t *data);
uint8_t twi_master_receive_from(uint8_t addr, char *buffer, uint8_t len);
uint8_t twi_master_transmit_to(uint8_t addr, char *buffer, uint8_t len);
uint8_t twi_set_read_address(uint8_t addr);
uint8_t twi_write_data(uint8_t byte);
uint8_t twi_set_write_address(uint8_t addr);

#endif