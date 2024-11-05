#include <stdint.h>
//
// Created by vincent on 11/1/24.
//

#include "periph/gpio.h"
#include "board.h"
#include "led_control.h"

// Initialize GPIO for each LED
void led_control_init(void) {
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_init(LED2_PIN, GPIO_OUT);
    gpio_init(LED3_PIN, GPIO_OUT);
}

// Turn on a specific LED
void led_control_turn_on(uint8_t led) {
    switch (led) {
        case 0: gpio_set(LED0_PIN); break;
        case 1: gpio_set(LED1_PIN); break;
        case 2: gpio_set(LED2_PIN); break;
        case 3: gpio_set(LED3_PIN); break;
        default: break;
    }
}

// Turn off a specific LED
void led_control_turn_off(uint8_t led) {
    switch (led) {
        case 0: gpio_clear(LED0_PIN); break;
        case 1: gpio_clear(LED1_PIN); break;
        case 2: gpio_clear(LED2_PIN); break;
        case 3: gpio_clear(LED3_PIN); break;
        default: break;
    }
}

// Toggle a specific LED
void led_control_toggle(uint8_t led) {
    switch (led) {
        case 0: gpio_toggle(LED0_PIN); break;
        case 1: gpio_toggle(LED1_PIN); break;
        case 2: gpio_toggle(LED2_PIN); break;
        case 3: gpio_toggle(LED3_PIN); break;
        default: break;
    }
}

// Get the current state of a specific LED
int led_control_get_state(uint8_t led) {
    switch (led) {
        case 0: return gpio_read(LED0_PIN);
        case 1: return gpio_read(LED1_PIN);
        case 2: return gpio_read(LED2_PIN);
        case 3: return gpio_read(LED3_PIN);
        default: return -1;  // Invalid LED index
    }
}
