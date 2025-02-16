//
// Created by vincent on 2/9/25.
//

#ifndef CONFIG_CONSTANTS_H
#define CONFIG_CONSTANTS_H

/* This file defines all the lengths for the app configurations.
 * All these lengths are in characters, with 1 char = 1 byte
 * The comment behind each of these is a short description with the actual length in square brackets.
 * If there is no number in square brackets it is an artificial limit.
 * The actual length is the length of the string that is stored using this definition.
 * E.g.:
 * #define ENGLISH_ABC 30           // Defines the number of letters in the english alphabet. [26]
 *
 * The english alphabet has 26 letters, but it is defined with 30 here to allow for extra padding, '\n', etc.
 */

#define BOT_TOKEN_LENGTH 50         // The length of the telegram bot token. [47]
#define MAX_CHAT_IDS 10             // The maximum number of telegram chats.
#define CHAT_ID_LENGTH 12           // The length of a single telegram chat id. [11]
#define CHAT_NAME_LENGTH 15         // The length of the associated first name to the chat id.
#define URL_LENGTH 30               // The length of the telegram bot url. [29]
#define ADDRESS_LENGTH 40           // The length of the IPv6 address of the CoAP server, enough space for any IPv6 address. [39]
#define PORT_LENGTH 5               // The length of the CoAP server port. [4]
#define URI_PATH_LENGTH 20          // The length of the CoAP server endpoint.
#define MESSAGE_DATA_LENGTH 40      // The maximum size of the actual message payload.


/* If any of the required configuration variables is not set during building this will make sure to initialize these
 * variables with some default values.
 */

#ifndef TEMPERATURE_NOTIFICATION_INTERVAL
#define TEMPERATURE_NOTIFICATION_INTERVAL 5
#endif

#ifndef ENABLE_LED_FEEDBACK
#define ENABLE_LED_FEEDBACK 0
#endif

#ifndef TELEGRAM_SERVER_URL
#define TELEGRAM_SERVER_URL "https://api.telegram.org/bot"
#endif

#ifndef COAP_SERVER_ADDRESS
#define COAP_SERVER_ADDRESS "::1"
#endif

#ifndef COAP_SERVER_PORT
#define COAP_SERVER_PORT "5683"
#endif

#ifndef COAP_SERVER_URI_PATH
#define COAP_SERVER_URI_PATH "/message"
#endif

/* Throw an error if the telegram bot token or chat ids is missing completely.
 * This will throw an error at build time.
 */
#ifndef TELEGRAM_BOT_TOKEN
#error "TELEGRAM_BOT_TOKEN is not defined!"
#endif

#ifndef TELEGRAM_CHAT_IDS
#error "TELEGRAM_CHAT_IDS is not defined!"
#endif

#endif //CONFIG_CONSTANTS_H
