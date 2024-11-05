//
// Created by vincent on 11/1/24.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shell.h"
#include "cmd_control.h"
#include "led_control.h"

#define MAX_LED_INDEX 4  // Maximum LED index

// Helper function to parse and execute action on multiple LEDs
static int execute_led_action(const char *action, int leds[], int led_count) {
    for (int i = 0; i < led_count; i++) {
        if (strcmp(action, "on") == 0) {
            led_control_turn_on(leds[i]);
            printf("LED %d turned ON\n", leds[i] + 1);
        }
        else if (strcmp(action, "off") == 0) {
            led_control_turn_off(leds[i]);
            printf("LED %d turned OFF\n", leds[i] + 1);
        }
        else if (strcmp(action, "toggle") == 0) {
            led_control_toggle(leds[i]);
            int state = led_control_get_state(leds[i]);
            printf("LED %d toggled %s\n", leds[i] + 1, state ? "OFF" : "ON"); //LED*_PIN value is 1 for OFF and 0 for ON
        }
        else {
            printf("Invalid action: %s\n", action);
            return 1;
        }
    }
    return 0;
}

// Command to turn on specified LEDs
int cmd_led_on(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: led_on <1-4> <1-4> ...\n");
        return 1;
    }

    int leds[MAX_LED_INDEX];
    int led_count = 0;

    for (int i = 1; i < argc; i++) {
        int led = atoi(argv[i]) - 1;  // Convert to 0-based index
        if (led < 0 || led >= MAX_LED_INDEX) {
            printf("Invalid LED index: %d\n", led + 1);
            return 1;
        }
        leds[led_count++] = led;
    }

    return execute_led_action("on", leds, led_count);
}

// Command to turn off specified LEDs
int cmd_led_off(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: led_off <1-4> <1-4> ...\n");
        return 1;
    }

    int leds[MAX_LED_INDEX];
    int led_count = 0;

    for (int i = 1; i < argc; i++) {
        int led = atoi(argv[i]) - 1;  // Convert to 0-based index
        if (led < 0 || led >= MAX_LED_INDEX) {
            printf("Invalid LED index: %d\n", led + 1);
            return 1;
        }
        leds[led_count++] = led;
    }

    return execute_led_action("off", leds, led_count);
}

// Command to toggle specified LEDs
int cmd_led_toggle(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: led_toggle <1-4> <1-4> ...\n");
        return 1;
    }

    int leds[MAX_LED_INDEX];
    int led_count = 0;

    for (int i = 1; i < argc; i++) {
        int led = atoi(argv[i]) - 1;  // Convert to 0-based index
        if (led < 0 || led >= MAX_LED_INDEX) {
            printf("Invalid LED index: %d\n", led + 1);
            return 1;
        }
        leds[led_count++] = led;
    }

    return execute_led_action("toggle", leds, led_count);
}

// Shell command list
static const shell_command_t shell_commands[] = {
    { "led_on", .handler=cmd_led_on, .desc="Turn on LEDs (Usage: led_on <1-4> <1-4> ...)" },
    { "led_off", .handler=cmd_led_off, .desc="Turn off LEDs (Usage: led_off <1-4> <1-4> ...)" },
    { "led_toggle", .handler=cmd_led_toggle, .desc="Toggle LEDs (Usage: led_toggle <1-4> <1-4> ...)" },
    { NULL, NULL, NULL }  // End of command list
};

// Initialize shell commands
void cmd_control_init(void) {
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
}
