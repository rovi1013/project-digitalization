//
// Created by jonas on 19.01.25.
//

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "net/gcoap.h"
#include "net/sock/udp.h"
#include "net/sock/util.h"
#include "od.h"
#include "uri_parser.h"
#include "net/utils.h"
#include "fmt.h"

//static bool _proxied = false;
//static char proxy_uri[64];
#define _LAST_REQ_PATH_MAX (64)
static char _last_req_path[_LAST_REQ_PATH_MAX];
uint16_t req_count = 0;

#ifndef CONFIG_URI_MAX
#define CONFIG_URI_MAX      128
#endif

//static sock_udp_ep_t _proxy_remote;
static char _proxy_uri[CONFIG_URI_MAX];

/* Retain request URI to re-request if response includes block. User must not
 * start a new request (with a new path) until any blockwise transfer
 * completes or times out. */
static char _last_req_uri[CONFIG_URI_MAX];

/* Last remote endpoint where an Observe request has been sent to */
//static sock_udp_ep_t obs_remote;

/* the token used for observing a remote resource */
//static uint8_t obs_req_token[GCOAP_TOKENLEN_MAX];

/* actual length of above token */
//static size_t obs_req_tkl = 0;

static gcoap_socket_type_t _get_tl(const char *uri)
{
    if (!strncmp(uri, "coaps", 5)) {
        return GCOAP_SOCKET_TYPE_DTLS;
    }
    else if (!strncmp(uri, "coap", 4)) {
        return GCOAP_SOCKET_TYPE_UDP;
    }
    return GCOAP_SOCKET_TYPE_UNDEF;
}

static ssize_t _sending(uint8_t *buf, size_t len, const sock_udp_ep_t *remote,
                     void *ctx, gcoap_socket_type_t tl);

//static ssize_t _send(uint8_t *buf, size_t len, const sock_udp_ep_t *remote,
                     //void *ctx, gcoap_socket_type_t tl);



// Initialize CoAP Control
void coap_control_init(void) {
    puts("Initializing coap control");
}

/*  */
int _print_usage(char **argv) {
    printf("Usage: %s [get|post]\n", argv[0]);
    return 1;
}

/*
 * Response callback.
 */
static void _resp_handler(const gcoap_request_memo_t *memo, coap_pkt_t* pdu,
                          const sock_udp_ep_t *remote)
{
    (void)remote;       /* not interested in the source currently */

    if (memo->state == GCOAP_MEMO_TIMEOUT) {
        printf("gcoap: timeout for msg ID %02u\n", coap_get_id(pdu));
        return;
    }
    else if (memo->state == GCOAP_MEMO_RESP_TRUNC) {
        /* The right thing to do here would be to look into whether at least
         * the options are complete, then to mentally trim the payload to the
         * next block boundary and pretend it was sent as a Block2 of that
         * size. */
        printf("gcoap: warning, incomplete response; continuing with the truncated payload\n");
    }
    else if (memo->state != GCOAP_MEMO_RESP) {
        printf("gcoap: error in response\n");
        return;
    }

    coap_block1_t block;
    if (coap_get_block2(pdu, &block) && block.blknum == 0) {
        puts("--- blockwise start ---");
    }

    char *class_str = (coap_get_code_class(pdu) == COAP_CLASS_SUCCESS)
                            ? "Success" : "Error";
    printf("gcoap: response %s, code %1u.%02u", class_str,
                                                coap_get_code_class(pdu),
                                                coap_get_code_detail(pdu));
    if (pdu->payload_len) {
        unsigned content_type = coap_get_content_type(pdu);
        if (content_type == COAP_FORMAT_TEXT
                || content_type == COAP_FORMAT_LINK
                || coap_get_code_class(pdu) == COAP_CLASS_CLIENT_FAILURE
                || coap_get_code_class(pdu) == COAP_CLASS_SERVER_FAILURE) {
            /* Expecting diagnostic payload in failure cases */
            printf(", %u bytes\n%.*s\n", pdu->payload_len, pdu->payload_len,
                                                          (char *)pdu->payload);
        }
        else {
            printf(", %u bytes\n", pdu->payload_len);
            od_hex_dump(pdu->payload, pdu->payload_len, OD_WIDTH_DEFAULT);
        }
    }
    else {
        printf(", empty payload\n");
    }

    /* ask for next block if present */
    if (coap_get_block2(pdu, &block)) {
        if (block.more) {
            unsigned msg_type = coap_get_type(pdu);
            if (block.blknum == 0 && !strlen(_last_req_uri)) {
                puts("Path too long; can't complete blockwise");
                return;
            }
            uri_parser_result_t urip;
            uri_parser_process(&urip, _last_req_uri, strlen(_last_req_uri));
            if (*_proxy_uri) {
                gcoap_req_init(pdu, (uint8_t *)pdu->hdr, CONFIG_GCOAP_PDU_BUF_SIZE,
                               COAP_METHOD_GET, NULL);
            }
            else {
                gcoap_req_init(pdu, (uint8_t *)pdu->hdr, CONFIG_GCOAP_PDU_BUF_SIZE,
                               COAP_METHOD_GET, urip.path);
            }

            if (msg_type == COAP_TYPE_ACK) {
                coap_hdr_set_type(pdu->hdr, COAP_TYPE_CON);
            }
            block.blknum++;
            coap_opt_add_block2_control(pdu, &block);

            if (*_proxy_uri) {
                coap_opt_add_proxy_uri(pdu, urip.scheme);
            }

            int len = coap_opt_finish(pdu, COAP_OPT_FINISH_NONE);
            gcoap_socket_type_t tl = _get_tl(*_proxy_uri ? _proxy_uri : _last_req_uri);
            _sending((uint8_t *)pdu->hdr, len, remote, memo->context, tl);
        }
        else {
            puts("--- blockwise complete ---");
        }
    }
}

/* Parsing the endpoint */
bool _parse_endpoint(sock_udp_ep_t *remote, const char*addr_str, const char *port_str) {

    if (netutils_get_ipv4((ipv4_addr_t *)&remote->addr, addr_str) < 0) {
        puts("unable to parse IPv4 address");
        return false;
    }
    remote->netif = SOCK_ADDR_ANY_NETIF;
    remote->family = AF_INET;

    remote->port = atoi(port_str);
    if (remote->port == 0) {
        puts("unable to parse port");
        return false;
    }


    /*
    netif_t *netif;

    if (netutils_get_ipv6((ipv6_addr_t *)&remote->addr, &netif, addr_str) < 0) {
        puts("unable to parse destination address");
        return false;
    }
    remote->netif = netif ? netif_get_id(netif) : SOCK_ADDR_ANY_NETIF;
    remote->family = AF_INET6;

    remote->port = atoi(port_str);
    if (remote->port ==0) {
        puts("unable to parse destination port");
        return false;
    }
    */
    return true;
}

/* Sending the message */
size_t _send(uint8_t *buf, size_t len, char *addr_str, char *port_str, void *ctx, gcoap_socket_type_t tl) {
    size_t bytes_sent;
    sock_udp_ep_t *remote;
    sock_udp_ep_t new_remote;

    if (!_parse_endpoint(&new_remote, addr_str, port_str)) {
        puts("Parsing ist fehlgeschlagen");
        return 0;
    }
    remote = &new_remote;

    printf("Konvertierte Serveradresse: %d, %d, %d, %d\n", remote->addr.ipv4[0], remote->addr.ipv4[1], remote->addr.ipv4[2], remote->addr.ipv4[3]);
    printf("Konvertierter Port: %d\n", remote->port);

    bytes_sent = gcoap_req_send(buf, len, remote, NULL, _resp_handler, ctx, tl);

    printf("Gesendete Bytes: %d\n", bytes_sent);

    if (bytes_sent > 0) {
        req_count++;
    }
    return bytes_sent;
}



/* Coap Control */
int coap_control(int argc, char **argv) {
    char *method_codes[] = {"ping", "get", "post", "put"};
    uint8_t buf[CONFIG_GCOAP_PDU_BUF_SIZE];
    coap_pkt_t pdu;
    size_t len;


    int code_pos = -1;
    for (size_t i = 0; i < ARRAY_SIZE(method_codes); i++) {
        if (strcmp(argv[1], method_codes[i]) == 0) {
            code_pos = i;
        }
    }
    if (code_pos == -1) {
        return _print_usage(argv);
    }

    /* parse options */
    int apos = 2;
    /* ping must be confirmable */
    unsigned msg_type = (!code_pos ? COAP_TYPE_CON : COAP_TYPE_NON);
    if (argc > apos && strcmp(argv[apos], "-c") == 0) {
        msg_type = COAP_TYPE_CON;
        apos++;
    }

    if (((argc == apos + 2) && (code_pos == 0)) ||      /* ping */
        ((argc == apos + 3) && (code_pos == 1)) ||      /* get */
        ((argc == apos + 3 ||
            argc == apos + 4) && (code_pos > 1))) {     /* post or put */

        char *uri = NULL;
        int uri_len = 0;
        if (code_pos) {
            uri = argv[apos+2];
            uri_len = strlen(argv[apos+2]);
        }

        gcoap_req_init(&pdu, &buf[0], CONFIG_GCOAP_PDU_BUF_SIZE, code_pos, uri);
        coap_hdr_set_type(pdu.hdr, msg_type);

        /* Preperation of the Payload */
        memset(_last_req_path, 0, _LAST_REQ_PATH_MAX);
        if (uri_len < _LAST_REQ_PATH_MAX) {
            memcpy(_last_req_path, uri, uri_len);
        }

        size_t paylen = (argc == apos + 4) ? strlen(argv[apos+3]) : 0;
        if (paylen) {
            coap_opt_add_format(&pdu, COAP_FORMAT_TEXT);
            len = coap_opt_finish(&pdu, COAP_OPT_FINISH_PAYLOAD);
            if (pdu.payload_len >= paylen) {
                memcpy(pdu.payload, argv[apos+3], paylen);
                len += paylen;
            }
            else {
                puts("gcoap_cli: msg buffer too small");
                return 1;
            }
        }
        else {
            len = coap_opt_finish(&pdu, COAP_OPT_FINISH_NONE);
        }

        gcoap_socket_type_t tl = _get_tl(_last_req_uri);

        /* Sending the message */
        printf("gcoap_cli: sending msg ID %u, %u bytes\n", coap_get_id(&pdu), (unsigned) len);
        if (_send(&buf[0], len, argv[apos], argv[apos+1], NULL, tl) == 0) {
            puts("gcoap_cli: msg send failed");
        }

    }
    return 0;
}

static ssize_t _sending(uint8_t *buf, size_t len, const sock_udp_ep_t *remote,
                     void *ctx, gcoap_socket_type_t tl)
{
    ssize_t bytes_sent = gcoap_req_send(buf, len, remote, NULL, _resp_handler, ctx, tl);
    if (bytes_sent > 0) {
        req_count++;
    }
    return bytes_sent;
}
