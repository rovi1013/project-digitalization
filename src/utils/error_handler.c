//
// Created by vincent on 12/15/24.
//

#include "error_handler.h"

const char *get_error_message(const int error_code) {
    switch (error_code) {
        // General error codes -1x
        case ERROR_UNKNOWN: return "Unknown error";
        case ERROR_MEMORY_FAIL: return "Memory allocation failed";
        case ERROR_NO_SENSOR: return "Specified sensor not found";

        // CPU Temperature specific error codes -2x
        case ERROR_READ_FAIL: return "Failed to read temperature data";

        // CMD control specific error codes -3x
        case ERROR_INVALID_ARGS: return "Invalid arguments";

        // LED control specific error codes -4x
        case ERROR_LED_WRITE: return "Failed to write led data";

        default: return "Undefined error";
    }
}