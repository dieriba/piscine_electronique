#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>
#include <util/delay.h>

#define LOW 0
#define HIGH 1
#define DEBOUNCE_TIME 50


typedef uint8_t (*read_input_pin)(void);
typedef void (*button_callback)(void);

/*
 * Detect a stable button state change and execute a callback.
 *
 * button_state      : reads the current GPIO pin state (0 or 1).
 * prev_button_state : stores the last stable button state.
 * callback          : executed when the button transitions to LOW.
 *
 */
static inline void button_do_if_new_state(read_input_pin button_state, uint8_t *prev_button_state, button_callback callback)
{
    uint8_t prev_state    = *prev_button_state;
    uint8_t current_state = button_state();

    if (prev_state != current_state)
    {
        _delay_ms(DEBOUNCE_TIME);
        current_state = button_state();
        if (prev_state != current_state)
        {
            *prev_button_state = current_state;
            if (current_state == LOW)
                callback();
        }
    }
}

#endif