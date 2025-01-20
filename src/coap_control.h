//
// Created by jonas on 19.01.25.
//

#ifndef COAP_CONTROL_H
#define COAP_CONTROL_H
#include <net/gcoap.h>
#include "net/sock/udp.h"

void coap_control_init(void);
int coap_control(int argc, char **argv);


#endif //COAP_CONTROL_H
