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

//############################################################
//########################## SETTER ##########################
//############################################################

/**
 * Set the temperature notification interval.
 * @param interval Interval in minutes.
 */
void config_set_notification_interval(int interval);

/**
 * Toggle the LED Feedback on the board for CoAP requests.
 * @param toggle Either enable (true) or disable (false) the functionality
 */
void config_set_led_feedback(bool toggle);

/**
 * Change to telegram bot token. *
 * @param token Telegram bot token.
 */
void config_set_bot_token(const char *token);

/**
 * Check if the given name or ID exists in the current configuration.
 * - If either is found, the corresponding value is updated.
 * - If no match is found, a new entry is created.
 * @param name - The username of the chat.
 * @param id - The ID of the chat.
 */
void config_set_chat_id(const char *name, const char *id);

/**
 * Change the default telegram bot URL.
 * @param url New telegram bot URL.
 */
void config_set_telegram_url(const char *url);

/**
 * Update the IPv6 address used to connect to the CoAP server.
 * @param address New IPv6 address.
 */
void config_set_address(const char *address);

/**
 * Update the port used to connect to the CoAP server.
 * @param port New port.
 */
void config_set_port(const char *port);

/**
 * Update the endpoint used to connect to the CoAP server.
 * @param path New endpoint.
 */
void config_set_uri_path(const char *path);

//############################################################
//########################## GETTER ##########################
//############################################################

/**
 * Get the temperature notification interval configuration.
 * @return Interval in minutes.
 */
int config_get_notification_interval(void);

/**
 * Get the LED Feedback configuration on the board for CoAP requests.
 * @return Either enabled (true) or disabled (false)
 */
bool config_get_led_feedback(void);

/**
 * Get the telegram bot token configuration.
 * @return Telegram bot token.
 */
const char* config_get_bot_token(void);

/**
 * Get a chat ID by its index in chat_ids.
 * @param index Index of the ID.
 * @return Chat ID.
 */
const char* config_get_chat_id(int index);

/**
 * Get a chat ID by tha associated username.
 * @param name The username.
 * @return Chat ID.
 */
const char* config_get_chat_id_by_name(const char *name);

/**
 * Get all chat IDs from the chat entries currently saved in chat_ids.
 * @return Comma seperated chat IDs.
 */
const char* config_get_chat_ids_string(void);

/**
 * Get the telegram bot URL configuration.
 * @return Telegram bot URL.
 */
const char* config_get_telegram_url(void);

/**
 * Get the CoAP server IPv6 address configuration.
 * @return CoAP IPv6 address.
 */
const char* config_get_address(void);

/**
 * Get the CoAP server port configuration.
 * @return CoAP Port.
 */
const char* config_get_port(void);

/**
 * Get the CoAP server endpoint configuration.
 * @return CoAP endpoint.
 */
const char* config_get_uri_path(void);

//############################################################
//######################### REMOVER ##########################
//############################################################

/**
 * Remove a chat entry from the configuration.
 * @param id_or_name Either ID or username.
 */
void config_remove_chat_by_id_or_name(const char *id_or_name);

#endif //CONFIGURATION_H
