//
// Created by jonas on 19.01.25.
//

#ifndef COAP_CONTROL_H
#define COAP_CONTROL_H
#include <net/gcoap.h>
#include "net/sock/udp.h"

#define MAX_CHAT_IDS 20
#define CHAT_ID_LENGTH 12
#define BOT_TOKEN_LENGTH 50

extern const char telegram_bot_token[BOT_TOKEN_LENGTH];
extern const char chat_ids[MAX_CHAT_IDS][CHAT_ID_LENGTH];

void coap_control_init(void);
int coap_control(int argc, char **argv);


#endif //COAP_CONTROL_H
