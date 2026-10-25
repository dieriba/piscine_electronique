#include "interrupt.h"
#include "pca955.h"
#include "port.h"
#include "twi.h"
#include "uart.h"

#include <stdint.h>
#include <util/delay.h>

#define pow2(pow) (1 << (pow))
#define DEBOUNCE_TIME_MS 15
#define WAIT_MS 1000
#define pow2_mod(numerator, modulus) ((numerator) & ((modulus) - 1))
#define POW pow2(3)

uint8_t button_previous_state;
uint8_t value;

void i2c_init(void)
{
    twi_set_prescaler1();
    twi_set_bitrate(72);
}

void init_interrupt()
{
    pd7_pull_up_enable();
    interrupt_pcint_group2_enable();
    interrupt_pd7_enable();
    interrupt_global_enable();
}

void init_io_pca955()
{
    pca955_set_port0_0_as_input();
    pca955_set_port0_1_as_output();
    pca955_set_port0_2_as_output();
    pca955_set_port0_3_as_output();
    pca955_set_p0_pin_state(0xff);
    button_previous_state = pd7_read();
}

void display_value_led(uint8_t value)
{
    pca955_set_p0_pin_state(~(value << 1) | 1);
}

SIGNAL(PCINT2_vect)
{
    uint8_t current_state = pd7_read();
    _delay_ms(DEBOUNCE_TIME_MS);
    uint8_t new_state = pd7_read();
    if (button_previous_state != new_state && current_state == new_state)
    {
        button_previous_state = current_state;
        if (current_state == 0)
        {
            value = pow2_mod(value + 1, POW);
            display_value_led(value);
        }
    }
}

int main()
{
    uart_init_tx_rx(SENDER);
    i2c_init();
    init_interrupt();
    init_io_pca955();
    while (1)
    {
    }
}