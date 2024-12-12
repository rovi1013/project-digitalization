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

// Write a value to an LED
static int led_saul_write(const uint8_t led_id, const int16_t value) {
    saul_reg_t *dev = saul_reg_find_nth(led_id);
    if (!dev) {
        printf("LED with ID %u not found\n", led_id);
        return -1;
    }

    const phydat_t data = { .val = { value, 0, 0 }, .scale = 0, .unit = UNIT_UNDEF };

    if (saul_reg_write(dev, &data) < 0) {
        printf("Failed to write to LED with ID %u\n", led_id);
        return -1;
    }

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
    const uint8_t value = atoi(action);
    return led_saul_write(led_id, value);
}

// Initialize LEDs
void led_control_init(void) {
    printf("LED control initialized using SAUL\n");
}
