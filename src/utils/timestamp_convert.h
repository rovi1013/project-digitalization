//
// Created by vincent on 12/15/24.
//

#ifndef TIMESTAMP_CONVERT_H
#define TIMESTAMP_CONVERT_H

#include <stdint.h>

/**
 * Converts a standard timestamp from microseconds to the format [hh:mm:ss]
 * @param timestamp The timestamp in microseconds
 * @param buffer Pointer to the formatted output
 * @param buffer_size Size of the output
 */
void format_timestamp(uint32_t timestamp, char *buffer, size_t buffer_size);

#endif //TIMESTAMP_CONVERT_H
