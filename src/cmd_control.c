//
// Created by vincent on 11/1/24.
//

#include <stdio.h>
#include <stdlib.h>

#include "shell.h"
#include "led_control.h"
#include "cpu_temperature.h"
#include "cmd_control.h"
#include "utils/error_handler.h"

// Handle LED control commands
static int led_control(const int argc, char **argv) {
    if (argc != 3) {
        printf("Error: %s\n", get_error_message(ERROR_INVALID_ARGS));
        puts("Usage: led <#led> <on/off/brightness>");
        return ERROR_INVALID_ARGS;
    }

    const uint8_t led_id = atoi(argv[1]);
    return led_control_execute(led_id, argv[2]);
}

// Handle CPU temperature commands
static int cpu_temp_control(const int argc, char **argv) {
    (void)argv;

    if (argc != 1) {
        printf("Error: %s\n", get_error_message(ERROR_INVALID_ARGS));
        puts("Usage: cpu-temp");
        return ERROR_INVALID_ARGS;
    }

    const cpu_temperature_t temp = cpu_temperature_execute();
    cpu_temperature_print(&temp);

    return 0;
}

// Shell commands array
static const shell_command_t cmd_control_shell_commands[] = {
    { "led", "Control LEDs (e.g., 'led 0 on')", led_control },
    { "cpu-temp", "Get CPU temperature (e.g., 'cpu-temp')", cpu_temp_control },
    { NULL, NULL, NULL } // End marker
};

// Initialize command control
void cmd_control_init(void) {
    puts("Initializing command control shell");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(cmd_control_shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
}



