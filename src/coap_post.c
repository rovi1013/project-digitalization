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

#include "configuration.h"
#include "utils/error_handler.h"

#define CONFIG_PASSWORD "password123"

coap_hdr_t coap_buffer[COAP_BUF_SIZE];  // Shared buffer for CoAP request
static bool coap_response_status = false;

// Set CoAP response handler status
void set_coap_response_status(const bool is_done) {
    coap_response_status = is_done;
}

// Get CoAP response handler status
bool get_coap_response_status(void) {
    return coap_response_status;
}

void process_config_command(const char *message) {
    char buffer[COAP_BUF_SIZE];
    snprintf(buffer, sizeof(buffer), "%s", message);
    buffer[COAP_BUF_SIZE - 1] = '\0';

    if (strlen(message) >= COAP_BUF_SIZE) {
        printf("Warning: message too long\n");
        return;
    }

    char *token = strtok(buffer, " ");
    if (!token || strcmp(token, "config") != 0) {
        printf("Ungültiges Format.\n");
        return;
    }

    // Check
    char *password = strtok(NULL, " ");
    if (!password || strcmp(password, CONFIG_PASSWORD) != 0) {
        printf("Falsches Passwort.\n");
        return;
    }

    // Variable lesen
    char *variable = strtok(NULL, " ");
    if (!variable) {
        printf("Fehlende Variable.\n");
        return;
    }

    // Werte auslesen
    char *value1 = strtok(NULL, " ");
    char *value2 = strtok(NULL, " ");

    if (strcmp(variable, "set-chat") == 0) {
        if (!value1 || !value2) {
            printf("Fehlende Werte für set-chat.\n");
            return;
        }
        printf("Setze Chat %s mit ID %s\n", value1, value2);
    }
    else {
        if (!value1) {
            printf("Fehlender Wert für %s.\n", variable);
            return;
        }
        if (strcmp(variable, "interval") == 0) {
            int minutes = atoi(value1);
            printf("Setze Intervall auf %d Minuten\n", minutes);

        }
        else if (strcmp(variable, "feedback") == 0) {
            int feedback = atoi(value1);
            printf("Setze Feedback auf %d\n", feedback);

        }
        else if (strcmp(variable, "bot-token") == 0) {
            printf("Setze Bot-Token: %s\n", value1);

        }
        else if (strcmp(variable, "remove-chat") == 0) {
            printf("Entferne Chat: %s\n", value1);

        }
        else if (strcmp(variable, "telegram-url") == 0) {
            printf("Setze Telegram-URL: %s\n", value1);

        }
        else if (strcmp(variable, "address") == 0) {
            printf("Setze IPv6-Adresse: %s\n", value1);

        }
        else if (strcmp(variable, "port") == 0) {
            int port = atoi(value1);
            printf("Setze Port auf %d\n", port);

        }
        else if (strcmp(variable, "uri-path") == 0) {
            printf("Setze URI-Pfad: %s\n", value1);

        }
        else {
            printf("Ungültige Konfigurationsanweisung.\n");
        }
    }
}

void config_control(coap_pkt_t *pkt) {
    if (pkt->payload_len > 0) {
        char response[COAP_BUF_SIZE];

        /*
        snprintf(response, pkt->payload_len, "%s", pkt->payload);
        response[pkt->payload_len] = '\0';
        */

        memcpy(response, pkt->payload, pkt->payload_len);


        printf("Received REsponse: %s\n", response);

        if (strncmp(response, "[", 1) == 0) {
            char *msg = strtok(response, "[],");
            while (msg) {
                //printf("%s\n", msg);
                puts("mist");
                process_config_command(msg);
                msg = strtok(NULL, "[],");
            }
        }
    }

}

// Response handler for CoAP requests
static void coap_response_handler(const gcoap_request_memo_t *memo, coap_pkt_t *pkt, const sock_udp_ep_t *remote) {
    (void)remote;
    set_coap_response_status(false);

    // Get the context
    coap_request_context_t *req_ctx = memo->context;
    if (!req_ctx) {
        handle_error(__func__, ERROR_NULL_POINTER);
        return;
    }

    // Handle timeouts
    if (memo->state == GCOAP_MEMO_TIMEOUT) {
        handle_error(__func__, ERROR_COAP_TIMEOUT);
        return;
    }

    const unsigned msg_type = (pkt->hdr->ver_t_tkl & 0x30) >> 4;

    /* Print Payload */
    if (pkt->payload_len > 0) {
        //printf("Received Response: %.*s\n", pkt->payload_len, (char *)pkt->payload);

        config_control(pkt);

        set_coap_response_status(true);
        return;
    }

    if (msg_type == COAP_TYPE_ACK) {
        if (pkt->hdr->code == COAP_CODE_EMPTY) {
            return;
        }
        set_coap_response_status(true);
        return;
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

            coap_hdr_set_type(next_block_pkt.hdr, COAP_TYPE_NON);
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
                printf("res: %d\n", res);
                handle_error(__func__, ERROR_COAP_SEND);
            }
        }
    }

    set_coap_response_status(true);
}

// Initialize and prepare the CoAP packet.
static int coap_prepare_packet(coap_pkt_t *pkt, const char *uri_path, const char *payload) {
    // Clear buffer before reuse
    memset(coap_buffer, 0, sizeof(coap_buffer));

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

// Send the CoAP request to the server.
static int coap_send_request(const coap_pkt_t *pkt) {
    // Prepare CoAP destination
    sock_udp_ep_t remote = { .family = AF_INET6 };
    if (ipv6_addr_from_str((ipv6_addr_t *)&remote.addr, config_get_address()) == NULL) {
        return ERROR_IPV6_FORMAT;
    }
    remote.netif = SOCK_ADDR_ANY_NETIF;
    remote.port = atoi(config_get_port());

    // Send the CoAP request
    coap_request_context_t req_ctx;
    req_ctx.message_id = pkt->hdr->id;
    req_ctx.uri_path = config_get_uri_path();

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

// Create a CoAP POST request with a given message and specific chat_id.
int coap_post_send(const char *message, const char *recipient) {
    set_coap_response_status(false);

    if (!message) {
        handle_error(__func__,ERROR_INVALID_ARGUMENT);
        return ERROR_INVALID_ARGUMENT;
    }

    char uri_path[URI_PATH_LENGTH + 1];
    char payload[COAP_BUF_SIZE];

    // Step 1: Build URI Path
    snprintf(uri_path, URI_PATH_LENGTH + 1, "%s", config_get_uri_path());

    // Step 2: Determine the chat ID(s)
    const char *chat_ids;
    if (strcmp(recipient, "all") != 0) {
        chat_ids = config_get_chat_id_by_name(recipient);
        if (!chat_ids) {
            handle_error(__func__, ERROR_CHAT_ID_NOT_FOUND);
            return ERROR_CHAT_ID_NOT_FOUND;
        }
    } else {
        chat_ids = config_get_chat_ids_string();  // Default: Send to all
    }

    // Step 3: Build Payload
    snprintf(payload, sizeof(payload), "url=%s&token=%s&chat_ids=%s&text=%s",
             config_get_telegram_url(), config_get_bot_token(), chat_ids, message);

    // Step 4: Prepare CoAP Packet
    coap_pkt_t pkt;
    if (coap_prepare_packet(&pkt, uri_path, payload) != COAP_PKT_SUCCESS) {
        return ERROR_COAP_INIT;
    }
    handle_error(__func__, COAP_PKT_SUCCESS);

    // Step 5: Send Request
    return coap_send_request(&pkt);
}

/* Sending a post request to websocket to make a get-request to fetch updates */
int coap_post_get_updates(void) {
    set_coap_response_status(false);

    char uri_path[URI_PATH_LENGTH + 1];
    char payload[COAP_BUF_SIZE];

    // Step 1: Build URI Path
    snprintf(uri_path, URI_PATH_LENGTH + 1, "%s", "/update");

    // Step 2: Build Payload
    snprintf(payload, sizeof(payload), "url=%s&token=%s", config_get_telegram_url(), config_get_bot_token());

    // Step 3: Prepare CoAP Packet
    coap_pkt_t pkt;
    if (coap_prepare_packet(&pkt, uri_path, payload) != COAP_PKT_SUCCESS) {
        return ERROR_COAP_INIT;
    }
    handle_error(__func__, COAP_PKT_SUCCESS);

    // Step 4: Send Request
    return coap_send_request(&pkt);
}