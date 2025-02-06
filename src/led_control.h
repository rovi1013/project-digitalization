//
// Created by vincent on 11/1/24.
//

#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <stdint.h>

/**
 * Write values to the LEDs via the SAUL interface
 * @param led_id The ID of the LED
 * @param action Turn a LED on/off or set a value [0,255]
 * @return Custom codes defined in error_handler.h.
 */
int led_control_execute(uint8_t led_id, const char *action);

/**
 * Initialize LED control
 */
void led_control_init(void);

#endif // LED_CONTROL_H