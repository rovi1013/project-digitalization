//
// Created by vincent on 11/1/24.
//

#include <stdio.h>
#include <stdlib.h>

#include "shell.h"
#include "ztimer.h"
#include "led_control.h"
#include "cpu_temperature.h"
#include "cmd_control.h"
#include "utils/error_handler.h"
#include "coap_post.h"

// Handle LED control commands
static int led_control(const int argc, char **argv) {
    if (argc != 3) {
        handle_error(__func__,ERROR_INVALID_ARGUMENT);
        puts("Usage: led <#led> <on/off/brightness>");
        return ERROR_INVALID_ARGUMENT;
    }

    const uint8_t led_id = atoi(argv[1]);
    return led_control_execute(led_id, argv[2]);
}

// Handle CPU temperature commands
static int cpu_temp_control(const int argc, char **argv) {
    (void)argv;

    if (argc != 1) {
        handle_error(__func__,ERROR_INVALID_ARGUMENT);
        puts("Usage: cpu-temp");
        return ERROR_INVALID_ARGUMENT;
    }

    char buffer_temp[CLASS_CMD_BUFFER_SIZE];
    cpu_temperature_t temp;
    cpu_temperature_get(&temp);
    cpu_temperature_formatter(&temp, CALL_FROM_CLASS_CMD, buffer_temp, CLASS_CMD_BUFFER_SIZE);
    puts(buffer_temp);

    return TEMP_SUCCESS;
}

static int coap_send_control(const int argc, char **argv) {
    if (argc != 2) {
        handle_error(__func__,ERROR_INVALID_ARGUMENT);
        puts("Usage: coap-test <message>");
        return ERROR_INVALID_ARGUMENT;
    }

    coap_request_t request;
    init_coap_request(&request);
    const int res = coap_post_send(&request, argv[1]);

    handle_error(__func__, res);

    if (res == COAP_SUCCESS) {
        led_control_execute(0,"on");
        ztimer_sleep(ZTIMER_MSEC, 500);
        led_control_execute(0,"off");
    }

    return 0;
}

// Shell commands array
static const shell_command_t cmd_control_shell_commands[] = {
    { "led", "Control LEDs (e.g., 'led 0 on')", led_control },
    { "cpu-temp", "Get CPU temperature (e.g., 'cpu-temp')", cpu_temp_control },
    { "coap-send", "Send a custom coap message.", coap_send_control },
    { NULL, NULL, NULL } // End marker
};

// Initialize command control
void cmd_control_init(void) {
    puts("Initializing command control shell");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(cmd_control_shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
}



