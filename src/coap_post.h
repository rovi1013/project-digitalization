//
// Created by jonas on 19.01.25.
//

#ifndef COAP_POST_H
#define COAP_POST_H

#include <stdbool.h>

#include "config_constants.h"
#include "net/gcoap.h"

/* Explanation of the composition of COAP_BUF_SIZE, the actual content and headers of the CoAP message:
 * 8 bytes: CoAP header (message type, token, MID)
 * URI_PATH_LENGTH + 4: URI path option storage
 * URL_LENGTH: Telegram API URL
 * BOT_TOKEN_LENGTH: Telegram bot token
 * MAX_CHAT_IDS * (CHAT_NAME_LENGTH + CHAT_ID_LENGTH + 1): Space for all chat IDs (plus 1 for commas)
 * MESSAGE_DATA_LENGTH: Actual message content
 * 20 bytes: Extra padding for option overhead
 */
#define COAP_BUF_SIZE (8 + URI_PATH_LENGTH + URL_LENGTH + BOT_TOKEN_LENGTH + (MAX_CHAT_IDS * (CHAT_NAME_LENGTH + CHAT_ID_LENGTH + 1)) + MESSAGE_DATA_LENGTH + 20)

/* Explanation of the composition of COAP_UPDATE_SIZE, the actual content and headers of the CoAP message:
 * 6 bytes: Configuration toggle notification interval
 * 3 bytes: Configuration toggle LED feedback
 * 2 + CHAT_ID_LENGTH: Space for remove chat ID
 * MAX_CHAT_IDS * (CHAT_NAME_LENGTH + CHAT_ID_LENGTH + 1): Space for all chat IDs (plus 1 for commas)
 */
#define COAP_UPDATE_SIZE (6 + 3 + (2 + CHAT_ID_LENGTH) + (MAX_CHAT_IDS * (CHAT_NAME_LENGTH + CHAT_ID_LENGTH + 1)))

/**
 * Store the context of a request
 */
typedef struct {
    uint16_t message_id;        /**< Message ID of a request */
    const char *uri_path;       /**< Pointer to the URI path of a request */
} coap_request_context_t;

/**
 * Set the status of the CoAP response handler.
 * @param is_done Status of the response handler.
 */
void set_coap_response_status(bool is_done);

/**
 * Get the current status of the CoAP response handler.
 * @return Whether CoAP response handler is finished or not.
 */
bool get_coap_response_status(void);

/**
 * Create and send a CoAP POST request with a given message.
 * @param message The message to send.
 * @param recipient Name of the person to send to. Set to 'all' to send to every chat.
 * @return Custom codes defined in error_handler.h.
 */
int coap_post_send(const char *message, const char *recipient);

/**
 * Create a CoAP POST request to get updates.
 * @return Custom codes defined in error_handler.h.
 */
int coap_post_get_updates(void);

//void config_control(coap_pkt_t *pkt);

#endif //COAP_POST_H
