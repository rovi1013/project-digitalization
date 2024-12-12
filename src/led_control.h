//
// Created by vincent on 11/1/24.
//

#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <stdint.h>

// Execute LED actions
int led_control_execute(uint8_t led_id, const char *action);

// Initialize LEDs
void led_control_init(void);

#endif // LED_CONTROL_H