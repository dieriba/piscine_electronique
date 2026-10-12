#include "port.h"
#include <util/delay.h>
#include <avr/io.h>

#define DEBOUNCE_TIME 25 /* time to wait while "de-bouncing" button */
#define LOCK_INPUT_TIME 100 /* time to wait after a button press */


void init_port()
{
    pd2_pull_up_enable();
    pb0_output();
    pb0_low();
}

int main()
{
    init_port();
    uint8_t last_input_pin_state = pd2_read();
    while (1)
    {

        uint8_t current_input_pin_state = pd2_read();
        if (current_input_pin_state != last_input_pin_state) {
            _delay_ms(DEBOUNCE_TIME);
            current_input_pin_state = pd2_read();
            if (current_input_pin_state != last_input_pin_state) {
                last_input_pin_state = current_input_pin_state;
                pb0_toggle();
            }
        }
        _delay_ms(LOCK_INPUT_TIME);
    }
}