//
// Created by vincent on 12/15/24.
//

#include <stdio.h>
#include "error_handler.h"

#include <stdint.h>

// Error lookup table
typedef struct {
    error_code_t code;
    const char *message;
    const char *log_level;
} error_entry_t;

static const error_entry_t error_table[] = {
#define X(code, message, log_level) { code, message, log_level },
    ERROR_LIST
    #undef X
};

// Find error message and log level based on error code
const error_entry_t *get_error_entry(const error_code_t error_code) {
    for (uint8_t i = 0; i < sizeof(error_table) / sizeof(error_table[0]); i++) {
        if (error_table[i].code == error_code) {
            return &error_table[i];
        }
    }
    return &error_table[sizeof(error_table) / sizeof(error_table[0]) - 1]; // Default unknown error
}

// Error handler
void handle_error(const char *function_name, const error_code_t error_code) {
    const error_entry_t *entry = get_error_entry(error_code);
    fprintf(stderr, "%s %s: %s\n", entry->log_level, function_name, entry->message);
}

