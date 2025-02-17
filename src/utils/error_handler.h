//
// Created by vincent on 12/15/24.
//

#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

/**
 * Error definition
 */
#define ERROR_LIST \
X(COAP_SUCCESS, "CoAP message send successful to server", "[INFO]") \
X(COAP_PKT_SUCCESS, "CoAP package created successful", "[INFO]") \
X(LED_SUCCESS, "LED operation successful", "[INFO]") \
X(TEMP_SUCCESS, "Temperature operation successful", "[INFO]") \
X(ERROR_INVALID_ARGUMENT, "Invalid argument provided to function", "[ERROR]") \
X(ERROR_INVALID_ARG_INTERVAL, "Interval must be a positive number", "[ERROR]") \
X(ERROR_INVALID_ARG_FEEDBACK, "Feedback must be 0 (off) or 1 (on)", "[ERROR]") \
X(ERROR_INVALID_ARG_PORT, "Port must be a valid number (1-65535)", "[ERROR]") \
X(ERROR_COAP_INIT, "CoAP packet initialization failed", "[ERROR]") \
X(ERROR_COAP_URI_PATH, "Unable to append URI path in CoAP request", "[ERROR]") \
X(ERROR_COAP_PAYLOAD, "Payload appending to CoAP request failed", "[ERROR]") \
X(ERROR_COAP_TIMEOUT, "CoAP request timeout", "[ERROR]") \
X(ERROR_IPV6_FORMAT, "Invalid IPv6 address format encountered", "[ERROR]") \
X(ERROR_COAP_SEND, "CoAP request transmission failed", "[ERROR]") \
X(ERROR_CHAT_ID_NOT_FOUND, "Chat with this ID/person does not exist", "[ERROR]") \
X(ERROR_ALLOC_MEMORY_FAIL, "Memory allocation failure detected", "[ERROR]") \
X(ERROR_NO_SENSOR, "Sensor not found or unavailable", "[ERROR]") \
X(ERROR_TEMP_READ_FAIL, "Temperature data read operation failed", "[ERROR]") \
X(ERROR_LED_WRITE, "Unable to write LED state", "[ERROR]") \
X(ERROR_NULL_POINTER, "NULL pointer detected in function call", "[ERROR]") \
X(ERROR_CALLER_UNKNOWN, "Unknown caller function", "[ERROR]") \
X(ERROR_UNKNOWN, "An unknown error occurred", "[ERROR]")

/**
 * Convert the list of errors to an enum
 */
typedef enum {
#define X(code, message, log_level) code,
    ERROR_LIST
    #undef X
} error_code_t;

/**
 * Handles errors based on the error code.
 * Logs an appropriate message.
 * @param function_name Name of the function reporting the error.
 * @param error_code The error code to handle.
 */
void handle_error(const char *function_name, error_code_t error_code);

/**
 * Simple command to get the last error message
 * @return Last error message
 */
int get_last_error(void);

#endif // ERROR_HANDLER_H

