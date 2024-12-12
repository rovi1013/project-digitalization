//
// Created by vincent on 11/1/24.
//

#include <stdio.h>

#include "shell.h"
#include "led_control.h"
#include "cpu_temperature.h"
#include "sensor_mock.h"
#include "cmd_control.h"

#include <stdlib.h>

// Handle LED control commands
static int led_control(const int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: led <#led> <on/off/brightness>\n");
        return -1;
    }

    const uint8_t led_id = atoi(argv[1]);
    return led_control_execute(led_id, argv[2]);
}

// Handle CPU temperature commands
static int cpu_temp_control(const int argc, char **argv) {
    (void)argv;

    if (argc != 1) {
        printf("Usage: cpu-temp\n");
        return -1;
    }

    return cpu_temperature_execute();
}

// Handle mock sensor commands
static int sensor_mock_control(const int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: mock <temp/hum>\n");
        return -1;
    }

    return sensor_mock_execute(argv[1]);
}

// Shell commands array
static const shell_command_t cmd_control_shell_commands[] = {
    { "led", "Control LEDs (e.g., 'led 0 on')", led_control },
    { "cpu-temp", "Get CPU temperature (e.g., 'cpu-temp')", cpu_temp_control },
    { "mock", "Read mock sensor (e.g., 'mock temp')", sensor_mock_control },
    { NULL, NULL, NULL } // End marker
};

// Initialize command control
void cmd_control_init(void) {
    printf("Initializing command control shell\n");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(cmd_control_shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
}



