#include "pca955.h"
#include "twi.h"
#include "uart.h"

#include <util/delay.h>

#define WAIT_MS 1000

void i2c_init(void)
{
    twi_set_prescaler1();
    twi_set_bitrate(72);
}

int main()
{
    i2c_init();
    uart_init_tx_rx(SENDER);
    pca955_set_port0_3_as_output();
    while (1)
    {
        pca955_toggle_port0_3();
        _delay_ms(WAIT_MS);
    }
}