//
// Created by jonas on 19.01.25.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "net/gcoap.h"
#include "net/sock/udp.h"
#include "net/coap.h"
#include "coap_post.h"
#include "utils/error_handler.h"

static coap_hdr_t coap_buffer[COAP_BUF_SIZE];  // Shared buffer for CoAP request

/**
 * Initialize the CoAP request
 */
void init_coap_request(coap_request_t *request) {
    if (!request) { // Null pointer
        handle_error(__func__,ERROR_NULL_POINTER);
        return;
    }
    // Set the values from the C macros
    struct {
        char *dest;
        const char *src;
        size_t size;
    } field_map[] = {
        {request->url, COAP_SERVER_URL, URL_LENGTH},
        {request->address, COAP_SERVER_ADDRESS, ADDRESS_LENGTH},
        {request->port, COAP_SERVER_PORT, PORT_LENGTH},
        {request->uri_path, COAP_SERVER_URI_PATH, URI_PATH_LENGTH},
        {request->telegram_bot_token, TELEGRAM_BOT_TOKEN, BOT_TOKEN_LENGTH},
        {request->chat_ids, TELEGRAM_CHAT_IDS, MAX_CHAT_IDS * CHAT_ID_LENGTH}
    };
    for (size_t i = 0; i < sizeof(field_map) / sizeof(field_map[0]); i++) {
        snprintf(field_map[i].dest, field_map[i].size, "%s", field_map[i].src);
    }
}

/**
 * Response handler for CoAP requests
 */
static void coap_response_handler(const gcoap_request_memo_t *memo, coap_pkt_t *pkt, const sock_udp_ep_t *remote) {
    (void)remote;

    // Get the context
    coap_request_context_t *req_ctx = (coap_request_context_t *)memo->context;
    if (!req_ctx) {
        handle_error(__func__, ERROR_NULL_POINTER);
        return;
    }

    // Handle timeouts
    if (memo->state == GCOAP_MEMO_TIMEOUT) {
        handle_error(__func__, ERROR_COAP_TIMEOUT);
        return;
    }

    unsigned msg_type = (pkt->hdr->ver_t_tkl & 0x30) >> 4;

    if (msg_type == COAP_TYPE_ACK) {
        if (pkt->hdr->code == COAP_CODE_EMPTY) {
            return;
        }
    }

    // Blockwise response handling
    coap_block1_t block;
    if (coap_get_block2(pkt, &block)) {
        if (block.more) {
            coap_pkt_t next_block_pkt;
            gcoap_req_init(
                &next_block_pkt,
                (uint8_t *)coap_buffer,
                COAP_BUF_SIZE,
                COAP_METHOD_GET,
                req_ctx->uri_path
            );

            coap_hdr_set_type(next_block_pkt.hdr, COAP_TYPE_CON);
            coap_opt_add_block2_control(&next_block_pkt, &block);

            sock_udp_ep_t response_target = *remote;
            ssize_t res = gcoap_req_send(
                (uint8_t *)coap_buffer,
                next_block_pkt.payload_len,
                &response_target,
                NULL,
                coap_response_handler,
                req_ctx,
                GCOAP_SOCKET_TYPE_UDP
            );

            if (res < 0) {
                handle_error(__func__, ERROR_COAP_SEND);
            }
        }
    }
}

/**
 * Initialize and prepare the CoAP packet.
 */
static int coap_prepare_packet(coap_pkt_t *pkt, const char *uri_path, const char *payload) {
    // Initialize CoAP request
    const int result = gcoap_req_init(
        pkt,
        (uint8_t *)coap_buffer,
        COAP_BUF_SIZE,
        COAP_METHOD_POST,
        uri_path
    );
    if (result < 0) {
        handle_error(__func__,ERROR_COAP_INIT);
        return ERROR_COAP_INIT;
    }

    // Set message type to Confirmable (CON)
    coap_hdr_set_type(pkt->hdr, COAP_TYPE_CON);

    // Add payload
    if (coap_opt_finish(pkt, COAP_OPT_FINISH_PAYLOAD) < 0) {
        handle_error(__func__,ERROR_COAP_PAYLOAD);
        return ERROR_COAP_PAYLOAD;
    }
    size_t payload_len = strlen(payload);
    if (payload_len >= pkt->payload_len) { // Prevent buffer overflow
        handle_error(__func__, ERROR_COAP_PAYLOAD);
        return ERROR_COAP_PAYLOAD;
    }

    memcpy(pkt->payload, payload, payload_len);
    pkt->payload[payload_len] = '\0'; // Ensure null termination

    return COAP_PKT_SUCCESS;
}

/**
 * Send the CoAP request to the server.
 */
static int coap_send_request(const coap_pkt_t *pkt, const coap_request_t *request) {
    // Prepare CoAP destination
    sock_udp_ep_t remote = { .family = AF_INET6 };
    if (ipv6_addr_from_str((ipv6_addr_t *)&remote.addr, request->address) == NULL) {
        return ERROR_IPV6_FORMAT;
    }
    remote.netif = SOCK_ADDR_ANY_NETIF;
    remote.port = atoi(request->port);

    // Send the CoAP request
    coap_request_context_t req_ctx;
    req_ctx.message_id = pkt->hdr->id;
    req_ctx.uri_path = request->uri_path;

    ssize_t coap_response = gcoap_req_send(
        (uint8_t *) coap_buffer,
        pkt->payload_len,
        &remote,
        NULL,
        coap_response_handler,
        (void *)&req_ctx,
        GCOAP_SOCKET_TYPE_UDP
    );

    const int err = get_last_error();
    if (coap_response <= 0 || err == ERROR_COAP_SEND || err == ERROR_COAP_TIMEOUT || err == ERROR_NULL_POINTER) {
        return ERROR_COAP_SEND;
    }

    return COAP_SUCCESS;
}

/**
 * Create a CoAP POST request with a given message.
 * Sends the message.
 */
int coap_post_send(coap_request_t *request, const char *message) {
    if (!request || !message) {
        handle_error(__func__,ERROR_INVALID_ARGUMENT);
        return ERROR_INVALID_ARGUMENT;
    }

    char uri_path[URI_PATH_LENGTH + 1];
    char payload[COAP_BUF_SIZE];

    // Step 1: Build URI Path
    snprintf(uri_path, URI_PATH_LENGTH + 1, "%s", request->uri_path);

    // Step 2: Build Payload
    snprintf(payload, sizeof(payload), "url=%s&token=%s&chat_ids=%s&text=%s",
             request->url, request->telegram_bot_token, request->chat_ids, message);

    // Step 3: Prepare CoAP Packet
    coap_pkt_t pkt;
    if (coap_prepare_packet(&pkt, uri_path, payload) != COAP_PKT_SUCCESS) {
        return ERROR_COAP_INIT;
    }
    handle_error(__func__, COAP_PKT_SUCCESS);

    // Step 4: Send Request
    return coap_send_request(&pkt, request);
}