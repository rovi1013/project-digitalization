//
// Created by jonas on 19.01.25.
//

#ifndef COAP_CONTROL_H
#define COAP_CONTROL_H
#include <net/gcoap.h>
#include "net/sock/udp.h"

void coap_control_init(void);
size_t _send(uint8_t *buf, size_t len, char *addr_str, char *port_str);
bool _parse_endpoint(sock_udp_ep_t *remote, const char*addr_str, const char *port_str);
void _resp_handler(const gcoap_request_memo_t *memo, coap_pkt_t* pdu, const sock_udp_ep_t *remote);
int coap_control(int argc, char **argv);

#endif //COAP_CONTROL_H
