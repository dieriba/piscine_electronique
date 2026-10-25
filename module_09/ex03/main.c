#include "pca955.h"
#include "twi.h"
#include "uart.h"

#include <stdint.h>
#include <util/delay.h>

#define WAIT_MS 1000
uint8_t button_previous_state;
uint8_t value;

static const uint8_t PCA955_SEG_REPR_NUMBER[] = {
    0b111111,  // ZERO
    0b110,     // ONE
    0b1011011, // TWO
    0b1001111, // THREE
    0b1100110, // FOUR,
    0b1101101, // FIVE
    0b1111101, // SIX
    0b111,     // SEVEN
    0b1111111, // HEIGHT
    0b1101111  // NINE
};

void i2c_init(void)
{
    twi_set_prescaler1();
    twi_set_bitrate(72);
}

void init_io_pca955()
{
    pca955_set_p1_pin_cfg(0);
    pca955_set_p0_pin_cfg(0);
    pca955_set_p0_pin_state(0xFF);
    pca955_set_port0_7_state_high();
}

int main()
{
    uart_init_tx_rx(SENDER);
    i2c_init();
    init_io_pca955();
    for (uint8_t i = 0;; i = (i + 1) % 10)
    {
        pca955_set_p1_pin_state(PCA955_SEG_REPR_NUMBER[i]);
        _delay_ms(WAIT_MS);
    }
}