//
// Created by vincent on 12/15/24.
//

#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

// Error codes
#define ERROR_UNKNOWN -11
#define ERROR_MEMORY_FAIL -12
#define ERROR_NO_SENSOR -13
#define ERROR_READ_FAIL -22
#define ERROR_INVALID_ARGS -31
#define ERROR_LED_WRITE -41

// Return error code
const char *get_error_message(int error_code);

#endif //ERROR_HANDLER_H
