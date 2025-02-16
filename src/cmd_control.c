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
#include "configuration.h"

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

// Send a custom CoAP message
static int coap_send_control(const int argc, char **argv) {
    if (argc != 2) {
        handle_error(__func__,ERROR_INVALID_ARGUMENT);
        puts("Usage: coap-test <message>");
        return ERROR_INVALID_ARGUMENT;
    }

    const int res = coap_post_send(argv[1]);
    handle_error(__func__, res);

    if (res == COAP_SUCCESS) {
        if (app_config.enable_led_feedback) {
            led_control_execute(0, "on");
        }

        uint32_t wait_time = 1000;  // Max wait time
        while (!get_coap_response_status() && wait_time > 0) {
            printf("handler status: %s\n", get_coap_response_status() == 0 ? "Running" : "Finished");
            ztimer_sleep(ZTIMER_MSEC, 50);
            wait_time -= 50;
        }
        printf("handler status: %s\n", get_coap_response_status() == 0 ? "Running" : "Finished");

        if (app_config.enable_led_feedback) {
            led_control_execute(0, "off");
        }
    }

    return 0;
}

// Change Configuration during runtime
static int modify_config(const int argc, char **argv) {
    if (argc < 2 || argc > 4) {
        handle_error(__func__,ERROR_INVALID_ARGUMENT);
        return ERROR_INVALID_ARGUMENT;
    }

    const char* name = argv[1];
    const char* value = argv[2];
    // Print out help page
    if (strcmp(argv[1], "help") == 0) {
        puts("Usage:");
        puts("  config get                        (Get the current configuration)");
        puts("  config interval <minutes>         (Set temperature notification interval)");
        puts("  config feedback <0|1>             (Enable/disable LED feedback)");
        puts("  config bot-token <token>          (Set Telegram bot token)");
        puts("  config chat-id <name> <id>        (Set chat ID for a name)");
        puts("  config telegram-url <url>         (Set Telegram API URL)");
        puts("  config address <IPv6>             (Set CoAP server address)");
        puts("  config port <port>                (Set CoAP server port)");
        puts("  config uri-path <path>            (Set CoAP server URI path)");
    }
    // Handle 'config get' to print out all
    if (strcmp(argv[1], "get") == 0) {
        puts("============================================================");
        puts("Current Configuration:");
        puts("------------------------------------------------------------");
        printf("%-25s| %d\n", "  Notification Interval", config_get_notification_interval());
        printf("%-25s| %s\n", "  LED Feedback", config_get_led_feedback() ? "Enabled" : "Disabled");
        printf("%-25s| %s\n", "  Telegram Bot Token", "[HIDDEN]");  // Not really necessary
        printf("%-25s| %s\n", "  Telegram URL", config_get_telegram_url());
        printf("%-25s| %s\n", "  CoAP Server Address", config_get_address());
        printf("%-25s| %s\n", "  CoAP Server Port", config_get_port());
        printf("%-25s| %s\n", "  CoAP URI Path", config_get_uri_path());
        printf("%-25s| %s\n", "  Chat IDs", config_get_chat_ids_string());
        puts("============================================================");
        puts("");
    }
    else if (strcmp(name, "interval") == 0 || strcmp(name, "notification") == 0) {
        const int interval = atoi(value);
        if (interval <= 0) {
            handle_error(__func__, ERROR_INVALID_ARG_INTERVAL);
            return ERROR_INVALID_ARGUMENT;
        }
        config_set_notification_interval(interval);
        puts("Temperature notification interval set successful.");
    }
    else if (strcmp(name, "feedback") == 0 || strcmp(name, "led-feedback") == 0) {
        const int feedback = atoi(value);
        if (feedback != 0 && feedback != 1) {
            handle_error(__func__, ERROR_INVALID_ARG_FEEDBACK);
            return ERROR_INVALID_ARG_FEEDBACK;
        }
        config_set_led_feedback(feedback);
        puts("LED feedback set successful.");
    }
    else if (strcmp(name, "bot-token") == 0) {
        config_set_bot_token(value);
        puts("Bot token updated successful.");
    }
    else if (strcmp(name, "chat-id") == 0) {
        if (argc != 4) {
            puts("Usage: config chat-id <name> <id>");
            handle_error(__func__,ERROR_INVALID_ARGUMENT);
            return ERROR_INVALID_ARGUMENT;
        }
        config_set_chat_id(argv[2], argv[3]);
        puts("Chat ID set successful.");
    }
    else if (strcmp(name, "telegram-url") == 0) {
        config_set_telegram_url(value);
        puts("Telegram URL set successful.");
    }
    else if (strcmp(name, "address") == 0) {
        config_set_address(value);
        puts("Address set successful.");
    }
    else if (strcmp(name, "port") == 0) {
        const int port = atoi(value);
        if (port <= 0 || port > 65535) {
            handle_error(__func__,ERROR_INVALID_ARG_PORT);
            return ERROR_INVALID_ARG_PORT;
        }
        config_set_port(value);
        puts("Port set successful.");
    }
    else if (strcmp(name, "uri-path") == 0) {
        config_set_uri_path(value);
        puts("URI path set successful.");
    }

    return 0;
}

// Shell commands array
static const shell_command_t cmd_control_shell_commands[] = {
    { "led", "Control LEDs (e.g., 'led 0 on')", led_control },
    { "cpu-temp", "Get CPU temperature (e.g., 'cpu-temp')", cpu_temp_control },
    { "coap-send", "Send a custom coap message.", coap_send_control },
    { "config", "Change the configuration settings.", modify_config },
    { NULL, NULL, NULL } // End marker
};

// Initialize command control
void cmd_control_init(void) {
    puts("Initializing command control shell");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(cmd_control_shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
}



