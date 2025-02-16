//
// Created by vincent on 2/9/25.
//

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <stdbool.h>

#include "config_constants.h"

/**
 * Struct for telegram chats
 */
typedef struct {
    char first_name[CHAT_NAME_LENGTH];              /**< First name of the person associated to a chat */
    char chat_id[CHAT_ID_LENGTH];                   /**< Chat id of a chat */
} chat_entry_t;

/**
 * Struct for configuration settings
 */
typedef struct {
    int temperature_notification_interval;          /**< Notification interval */
    bool enable_led_feedback;                       /**< Led Feedback toggle */
    char bot_token[BOT_TOKEN_LENGTH];               /**< Telegram bot token */
    chat_entry_t chat_ids[MAX_CHAT_IDS];            /**< Telegram chat ids */
    char telegram_url[URL_LENGTH];                  /**< Telegram API URL */
    char address[ADDRESS_LENGTH];                   /**< CoAP server IPv6 address */
    char port[PORT_LENGTH];                         /**< CoAP server port */
    char uri_path[URI_PATH_LENGTH];                 /**< CoAP server URI path */
} config_t;

/**
 * Store configuration
 */
extern config_t app_config;

/**
 * Initialize and fill the config struct config_t.
 */
void config_init(void);

/**
 * Set the 'updated' flag of the configuration.
 * Can be used to trigger a reinitialization of the config_t struct.
 * @param is_up_to_date Set the 'updated' status.
 */
void config_set_update_flag(bool is_up_to_date);

/**
 * Get the 'updated' status of the current configuration.
 * @return Current 'is up to date'-status.
 */
bool config_get_update_flag(void);

// Setters
void config_set_notification_interval(int interval);
void config_set_led_feedback(bool enable);
void config_set_bot_token(const char *token);
void config_set_chat_id(const char *name, const char *id);
void config_set_telegram_url(const char *url);
void config_set_address(const char *address);
void config_set_port(const char *port);
void config_set_uri_path(const char *path);

// Getters
int config_get_notification_interval(void);
bool config_get_led_feedback(void);
const char* config_get_bot_token(void);
const char* config_get_chat_id(int index);
const char* config_get_chat_id_by_name(const char *name);
const char* config_get_chat_ids_string(void);
const char* config_get_telegram_url(void);
const char* config_get_address(void);
const char* config_get_port(void);
const char* config_get_uri_path(void);

#endif //CONFIGURATION_H
