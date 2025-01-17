//
// Created by vincent on 11/1/24.
//

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "saul_reg.h"
#include "phydat.h"
#include "led_control.h"
#include "utils/error_handler.h"

// Write a value to an LED
static int led_saul_write(const uint8_t led_id, const int16_t value) {
    saul_reg_t *dev = saul_reg_find_nth(led_id);

#ifdef BOARD_NATIVE
    (void) dev;
    if (value == 0 || value == 255) {
        printf("LED %u set to %s\n", led_id, value == 0 ? "off" : "on");
    } else {
        printf("LED %u set to %d\n", led_id, value);
    }
#else

    if (!dev) {
        printf("LED with ID %u not found\n", led_id);
        return ERROR_NO_SENSOR;
    }

    // Use phydat_t struct {val[], scale, unit} to write to a SAUL device
    const phydat_t data = { .val = { value, 0, 0 }, .scale = 0, .unit = UNIT_UNDEF };

    if (saul_reg_write(dev, &data) < 0) {
        printf("Failed to write to LED with ID %u\n", led_id);
        return ERROR_LED_WRITE;
    }

#endif

    return 0;
}

// Execute LED actions
int led_control_execute(uint8_t led_id, const char *action) {
    // SAUL LED ids on nrf52840dk start at 4
    led_id = led_id + 4;

    if (strcmp(action, "on") == 0) {
        return led_saul_write(led_id, 255); // 255 is ON
    }
    if (strcmp(action, "off") == 0) {
        return led_saul_write(led_id, 0); // 0 is OFF
    }
    // Use 8-bit integer to restrict value to 0-255
    const uint8_t value = atoi(action);
    return led_saul_write(led_id, value);
}

// Initialize LEDs
void led_control_init(void) {
    puts("LED control initialized using SAUL");
}
