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
#define ENDPOINT_LENGTH 20
#define MESSAGE_DATA_LENGTH 30
#define MAX_CHAT_IDS 10
#define CHAT_ID_LENGTH 12
#define BOT_TOKEN_LENGTH 50

typedef struct {
    char url[URL_LENGTH];
    char address[ADDRESS_LENGTH];
    char port[PORT_LENGTH];
    char endpoint[ENDPOINT_LENGTH];
    char data[MESSAGE_DATA_LENGTH];
    char telegram_bot_token[BOT_TOKEN_LENGTH];
    char chat_ids[MAX_CHAT_IDS][CHAT_ID_LENGTH];
} coap_request_t;

void coap_control_init(void);
int coap_control(int argc, char **argv);


#endif //COAP_CONTROL_H
