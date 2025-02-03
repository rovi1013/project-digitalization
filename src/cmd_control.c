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
#include "coap_control.h"

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

static int coap_test_control(const int argc, char **argv) {
    if (argc != 1) {
        printf("Error: %s\n", get_error_message(ERROR_INVALID_ARGS));
        puts("Usage: coap-test");
        return ERROR_INVALID_ARGS;
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
    
    (void)argv;
    
    coap_request_t request = init_coap_request();

    coap_control(argcNew, argvNew);

    return 0;
}

static int coap_post_control(const int argc, char **argv) {
    if (argc != 2) {
        printf("Error: %s\n", get_error_message(ERROR_INVALID_ARGS));
        puts("Usage: coap-post [Nachricht]");
        return ERROR_INVALID_ARGS;
    }

    char *command = argv[0];
    char *


    return 0;
}

// Shell commands array
static const shell_command_t cmd_control_shell_commands[] = {
    { "led", "Control LEDs (e.g., 'led 0 on')", led_control },
    { "cpu-temp", "Get CPU temperature (e.g., 'cpu-temp')", cpu_temp_control },
    { "coap-test", "Send a custom coap message.", coap_test_control },
    { "coap-post", "Send a coap message to a target destination", coap_post_control},
    { NULL, NULL, NULL } // End marker
};

// Initialize command control
void cmd_control_init(void) {
    puts("Initializing command control shell");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(cmd_control_shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
}



