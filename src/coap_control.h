//
// Created by jonas on 19.01.25.
//

#ifndef COAP_CONTROL_H
#define COAP_CONTROL_H
#include <net/gcoap.h>
#include "net/sock/udp.h"

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

void coap_control_init(void);
int coap_control(int argc, char **argv);


#endif //COAP_CONTROL_H
