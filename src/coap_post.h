//
// Created by jonas on 19.01.25.
//

#ifndef COAP_POST_H
#define COAP_POST_H

#define URL_LENGTH 35
#define ADDRESS_LENGTH 45
#define PORT_LENGTH 5
#define URI_PATH_LENGTH 20
#define MESSAGE_DATA_LENGTH 30
#define MAX_CHAT_IDS 10
#define CHAT_ID_LENGTH 12
#define BOT_TOKEN_LENGTH 50

/*
* 8 bytes: CoAP header (message type, token, MID)
* URI_PATH_LENGTH + 4: URI path option storage
* URL_LENGTH: Telegram API URL
* BOT_TOKEN_LENGTH: Telegram bot token
* MAX_CHAT_IDS * CHAT_ID_LENGTH: Space for all chat IDs
* MESSAGE_DATA_LENGTH: Actual message content
* 20 bytes: Extra padding for option overhead
*/
#define COAP_BUF_SIZE (8 + URI_PATH_LENGTH + 4 + URL_LENGTH + BOT_TOKEN_LENGTH + (MAX_CHAT_IDS * CHAT_ID_LENGTH) + MESSAGE_DATA_LENGTH + 20)

/**
 * Store the request information and data
 */
typedef struct {
    char url[URL_LENGTH];                           /**< Telegram API URL */
    char address[ADDRESS_LENGTH];                   /**< CoAP server IPv6 address */
    char port[PORT_LENGTH];                         /**< CoAP server port */
    char uri_path[URI_PATH_LENGTH];                 /**< CoAP server URI path */
    char telegram_bot_token[BOT_TOKEN_LENGTH];      /**< Telegram bot token */
    char chat_ids[MAX_CHAT_IDS*CHAT_ID_LENGTH];     /**< Telegram chat ids */
} coap_request_t;

/**
 * Store the context of a request
 */
typedef struct {
    uint16_t message_id;                            /**< Message ID of a request */
    const char *uri_path;                           /**< Pointer to the URI path of a request */
} coap_request_context_t;

/**
 * Init a coap_request_t struct with information and data
 * @param request Pointer to a coap_request_t struct
 */
void init_coap_request(coap_request_t *request);

/**
 * Create a CoAP POST request with a given message.
 * Sends the message.
 * @param request Pointer to the coap_request_t struct.
 * @param message The message to send.
 * @return Custom codes defined in error_handler.h.
 */
int coap_post_send(coap_request_t *request, const char *message);

#endif //COAP_POST_H
