//
// Created by vincent on 11/1/24.
//

#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <stdint.h> // For defining uint8_t type

// Initialize LED GPIOs
void led_control_init(void);

// Turn on a specified LED (0-3 for nRF52840 DK)
void led_control_turn_on(uint8_t led);

// Turn off a specified LED (0-3 for nRF52840 DK)
void led_control_turn_off(uint8_t led);

// Toggle a specified LED (0-3 for nRF52840 DK)
void led_control_toggle(uint8_t led);

// Get the current state of a specified LED (0-3 for nRF52840 DK)
int led_control_get_state(uint8_t led);

#endif // LED_CONTROL_H