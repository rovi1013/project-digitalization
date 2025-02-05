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

    cpu_temperature_t temp;
    cpu_temperature_get(&temp);
    cpu_temperature_print(&temp);

    return TEMP_SUCCESS;
}

static int coap_send_control(const int argc, char **argv) {
    if (argc != 2) {
        handle_error(__func__,ERROR_INVALID_ARGUMENT);
        puts("Usage: coap-test <message>");
        return ERROR_INVALID_ARGUMENT;
    }
    //char *command = argv[0];
    //char *type = "post";
    //char *addr = "fe80::a41d:b3f5:5212:392f";
    //char *addr = "fe80::6b96:48d0:4a13:27f5";
    //char *addr = "fe80::a74:adc8:de28:e5a6";
    //char *addr = "2001:470:7347:c822::1234";
    //char *port = "5683";
    //char *path = "/message";
    //char *data = "chat_ids=7779371199&text=Hello, CoAP!&token=";
    //char *data = "chat_id=7837794124&text=Gute Besserung wünscht dir das nrf52840dk!";
    //char *argvNew[] = {command, type, addr, port, path, data};
    //int argcNew = 6;

    coap_request_t request;
    init_coap_request(&request);
    const int res = coap_post_send(&request, argv[1]);

    handle_error(__func__, res);
    return 0;

    //const coap_request_t request = coap_control(argv[1]);
    //coap_request_t request = init_coap_request();
    //coap_control(argcNew, argvNew);

    //return 0;
}

// static int coap_post_control(const int argc, char **argv) {
//     if (argc != 2) {
//         handle_error(__func__,ERROR_INVALID_ARGUMENT);
//         puts("Usage: coap-post [Nachricht]");
//         return ERROR_INVALID_ARGUMENT;
//     }
//
//     char *command = argv[0];
//     char *
//
//
//     return 0;
// }

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



