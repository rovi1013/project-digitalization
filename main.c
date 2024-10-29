#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include "shell.h"
#include "periph/gpio.h"
#include "board.h"

// Initialize LEDs
void init_leds(void) {
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_init(LED2_PIN, GPIO_OUT);
    gpio_init(LED3_PIN, GPIO_OUT);
}

// Command to turn on an LED
// LED*_PIN value for ON is 0
int cmd_led_on(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: led_on <0|1|2|3>\n");
        return 1;
    }
    
    int led = atoi(argv[1]);
    switch (led) {
        case 1: gpio_write(LED0_PIN, 0); break;
        case 2: gpio_write(LED1_PIN, 0); break;
        case 3: gpio_write(LED2_PIN, 0); break;
        case 4: gpio_write(LED3_PIN, 0); break;
        default: printf("Invalid LED number\n"); return 1;
    }

    printf("LED %d is now ON\n", led);
    return 0;
}

// Command to turn off an LED
// LED*_PIN value for OFF is 1
int cmd_led_off(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: led_off <0|1|2|3>\n");
        return 1;
    }
    
    int led = atoi(argv[1]);
    switch (led) {
        case 1: gpio_write(LED0_PIN, 1); break;
        case 2: gpio_write(LED1_PIN, 1); break;
        case 3: gpio_write(LED2_PIN, 1); break;
        case 4: gpio_write(LED3_PIN, 1); break;
        default: printf("Invalid LED number\n"); return 1;
    }

    printf("LED %d is now OFF\n", led);
    return 0;
}

// Command to toggle an LED
int cmd_led_toggle(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: led_toggle <0|1|2|3>\n");
        return 1;
    }

    int led = atoi(argv[1]);
    switch (led) {
        case 1: gpio_toggle(LED0_PIN); break;
        case 2: gpio_toggle(LED1_PIN); break;
        case 3: gpio_toggle(LED2_PIN); break;
        case 4: gpio_toggle(LED3_PIN); break;
        default: printf("Invalid LED number\n"); return 1;
    }

    printf("LED %d toggled\n", led);
    return 0;
}

// Define shell commands
static const shell_command_t shell_commands[] = {
    {"led_on", .handler = cmd_led_on, .desc = "Turn on an LED (Usage: led_on <1|2|3|4>)"},
    {"led_off", .handler = cmd_led_off, .desc = "Turn off an LED (Usage: led_off <1|2|3|4>)"},
    {"led_toggle", .handler = cmd_led_toggle, .desc = "Toggle an LED (Usage: led_toggle <1|2|3|4>)"},
    {NULL, NULL, NULL}
};

int main(void) {
    // Initialize LEDs
    init_leds();

    // Start the shell
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    
    return 0;
}
