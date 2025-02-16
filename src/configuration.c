//
// Created by vincent on 2/9/25.
//

#include <string.h>
#include <stdio.h>

#include "configuration.h"
#include "utils/error_handler.h"

config_t app_config;

void config_init(void) {
    // Set default values (from CFLAGS)
    app_config.temperature_notification_interval = TEMPERATURE_NOTIFICATION_INTERVAL;
    app_config.enable_led_feedback = (ENABLE_LED_FEEDBACK == 1) ? true : false;
    snprintf(app_config.bot_token, BOT_TOKEN_LENGTH, "%s", TELEGRAM_BOT_TOKEN);
    snprintf(app_config.telegram_url, URL_LENGTH, "%s", TELEGRAM_SERVER_URL);
    snprintf(app_config.address, ADDRESS_LENGTH, "%s", COAP_SERVER_ADDRESS);
    snprintf(app_config.port, PORT_LENGTH, "%s", COAP_SERVER_PORT);
    snprintf(app_config.uri_path, URI_PATH_LENGTH, "%s", COAP_SERVER_URI_PATH);

    // Parse TELEGRAM_CHAT_IDS (Format: "UserName:123456789,...")
    char chat_ids_copy[MAX_CHAT_IDS*(CHAT_ID_LENGTH+1)];
    snprintf(chat_ids_copy, MAX_CHAT_IDS*(CHAT_ID_LENGTH+1), "%s", TELEGRAM_CHAT_IDS);
    chat_ids_copy[sizeof(chat_ids_copy) - 1] = '\0';  // Ensure null-termination

    const char *token = strtok(chat_ids_copy, ",");
    int index = 0;

    while (token && index < MAX_CHAT_IDS) {
        char *colon = strchr(token, ':');
        if (colon) {
            *colon = '\0';  // Split name and ID
            snprintf(app_config.chat_ids[index].first_name, CHAT_NAME_LENGTH, "%s", token);
            snprintf(app_config.chat_ids[index].chat_id, CHAT_ID_LENGTH, "%s", colon + 1);
        } else {
            // If no name is provided, use empty string for name
            app_config.chat_ids[index].first_name[0] = '\0';
            strncpy(app_config.chat_ids[index].chat_id, token, CHAT_ID_LENGTH);
        }

        token = strtok(NULL, ",");
        index++;
    }
}

//############################################################
//########################## SETTER ##########################
//############################################################

void config_set_notification_interval(const int interval) {
    app_config.temperature_notification_interval = interval;
}

void config_set_led_feedback(const bool toggle) {
    app_config.enable_led_feedback = toggle;
}

void config_set_bot_token(const char *token) {
    snprintf(app_config.bot_token, BOT_TOKEN_LENGTH, "%s", token);
}

void config_set_chat_id(const char *name, const char *id) {
    if (!name || !id) {
        handle_error(__func__, ERROR_INVALID_ARGUMENT);
        return;
    }

    // Check if the id or name already exists and update it
    for (int i = 0; i < MAX_CHAT_IDS; i++) {
        if (strcmp(app_config.chat_ids[i].first_name, name) == 0) {
            snprintf(app_config.chat_ids[i].chat_id, CHAT_ID_LENGTH, "%s", id);
            return;
        }
        if (strcmp(app_config.chat_ids[i].chat_id, id) == 0) {
            snprintf(app_config.chat_ids[i].first_name, CHAT_NAME_LENGTH, "%s", name);
            return;
        }
    }

    // If not found, add a new entry in the first empty slot
    for (int i = 0; i < MAX_CHAT_IDS; i++) {
        if (strlen(app_config.chat_ids[i].chat_id) == 0) {  // Empty slot found
            snprintf(app_config.chat_ids[i].first_name, CHAT_NAME_LENGTH, "%s", name);
            snprintf(app_config.chat_ids[i].chat_id, CHAT_ID_LENGTH, "%s", id);
            return;
        }
    }
}

void config_set_telegram_url(const char *url) {
    snprintf(app_config.telegram_url, URL_LENGTH, "%s", url);
}

void config_set_address(const char *address) {
    snprintf(app_config.address, ADDRESS_LENGTH, "%s", address);
}

void config_set_port(const char *port) {
    snprintf(app_config.port, PORT_LENGTH, "%s", port);
}

void config_set_uri_path(const char *path) {
    snprintf(app_config.uri_path, URI_PATH_LENGTH, "%s", path);
}

//############################################################
//########################## GETTER ##########################
//############################################################

int config_get_notification_interval(void) {
    return app_config.temperature_notification_interval;
}

bool config_get_led_feedback(void) {
    return app_config.enable_led_feedback;
}

const char* config_get_bot_token(void) {
    return app_config.bot_token;
}

const char* config_get_chat_id(const int index) {
    if (index < 0 || index >= MAX_CHAT_IDS) {
        return NULL; // Prevent out-of-bounds access
    }
    return app_config.chat_ids[index].chat_id;
}

const char* config_get_chat_id_by_name(const char *name) {
    for (int i = 0; i < MAX_CHAT_IDS; i++) {
        if (strcmp(app_config.chat_ids[i].first_name, name) == 0) {
            return app_config.chat_ids[i].chat_id;
        }
    }
    return NULL;
}

const char* config_get_chat_ids_string(void) {
    static char chat_ids_str[MAX_CHAT_IDS * (CHAT_ID_LENGTH + 2)];
    chat_ids_str[0] = '\0';

    for (int i = 0; i < MAX_CHAT_IDS; i++) {
        if (strlen(app_config.chat_ids[i].chat_id) > 0) {
            if (strlen(chat_ids_str) > 0) {
                strcat(chat_ids_str, ",");
            }
            strcat(chat_ids_str, app_config.chat_ids[i].chat_id);
        }
    }
    return chat_ids_str;
}


const char* config_get_telegram_url(void) {
    return app_config.telegram_url;
}

const char* config_get_address(void) {
    return app_config.address;
}

const char* config_get_port(void) {
    return app_config.port;
}

const char* config_get_uri_path(void) {
    return app_config.uri_path;
}

// Remove chat entries by ID or username
void config_remove_chat_by_id_or_name(const char *id_or_name) {
    if (!id_or_name) return;

    for (int i = 0; i < MAX_CHAT_IDS; i++) {
        if (strcmp(app_config.chat_ids[i].first_name, id_or_name) == 0) {
            app_config.chat_ids[i].first_name[0] = '\0';
            app_config.chat_ids[i].chat_id[0] = '\0';
            return;
        }
        if (strcmp(app_config.chat_ids[i].chat_id, id_or_name) == 0) {
            app_config.chat_ids[i].first_name[0] = '\0';
            app_config.chat_ids[i].chat_id[0] = '\0';
            return;
        }
    }
}
