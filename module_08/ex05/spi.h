#ifndef SPI_H
#define SPI_H
#include "port.h"
#include <avr/io.h>
#include <stdint.h>

static inline void spi_master_mode_set()
{
    /*SET SS output to ensure Master operation*/
    pb2_output();
    /* Set MOSI and SCK output, all others input */
    pb3_output();
    pb5_output();
    /* Enable SPI, Master, set clock rate fck/16 */
    SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}

static inline uint8_t spi_master_transmit_byte(char cData)
{
    /* Start transmission */
    SPDR = cData;
    /* Wait for transmission complete */
    while(!(SPSR & (1<<SPIF)))
        ;
    return SPDR;
}

static inline void spi_master_transmit_data(const void *data, uint16_t len)
{
    const char *p = data;
    for (uint16_t i = 0; i < len; i++)
    {
        spi_master_transmit_byte(p[i]);
    }
}

#endif
