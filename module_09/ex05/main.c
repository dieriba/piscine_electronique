#include "interrupt.h"
#include "pca955.h"
#include "timer.h"
#include "twi.h"
#include "uart.h"

#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>

#define VALUE_TO_COMPARE 15624
#define MAX_VALUE 10000

uint8_t button_previous_state;
uint8_t value;
uint16_t volatile count;
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
    pca955_set_port0_4_state_low();
    pca955_set_port0_5_state_low();
    pca955_set_port0_6_state_low();
    pca955_set_port0_7_state_low();
}

void display_number(uint16_t number, uint8_t digit)
{
    if (number > 9)
        display_number(number / 10, digit - 1);
    pca955_set_p1_pin_state(PCA955_SEG_REPR_NUMBER[number % 10]);

    switch (digit)
    {
    case 0:
        pca955_set_port0_4_state_high();
        break;
    case 1:
        pca955_set_port0_5_state_high();
        break;

    case 2:
        pca955_set_port0_6_state_high();
        break;

    case 3:
        pca955_set_port0_7_state_high();
        break;
    default:
        break;
    }

    switch (digit)
    {
    case 0:
        pca955_set_port0_4_state_low();
        break;

    case 1:
        pca955_set_port0_5_state_low();
        break;

    case 2:
        pca955_set_port0_6_state_low();
        break;

    case 3:
        pca955_set_port0_7_state_low();
        break;
    default:
        break;
    }
}

SIGNAL(TIMER1_COMPA_vect)
{
    count++;

    if (count >= MAX_VALUE)
    {
        count = 0;
    }
}

void init_timer1()
{
    timer1_set_mode_ctc();
    timer1_set_prescaler_1024();
    timer1_interrupt_on_match_a(VALUE_TO_COMPARE);
    interrupt_global_enable();
}

int main()
{
    uart_init_tx_rx(SENDER);
    i2c_init();
    init_io_pca955();
    init_timer1();
    while (1)
    {
        display_number(count, 3);
    }
}