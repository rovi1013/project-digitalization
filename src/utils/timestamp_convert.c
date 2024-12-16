//
// Created by vincent on 12/15/24.
//

#include <stdio.h>

#include "timestamp_convert.h"

void format_timestamp(const uint32_t timestamp, char *buffer, const size_t buffer_size) {
    const uint32_t total_seconds = timestamp / 1000000; // Convert microseconds to seconds
    const uint16_t hours = total_seconds / 3600;
    const uint16_t minutes = (total_seconds % 3600) / 60;
    const uint16_t seconds = total_seconds % 60;

    snprintf(buffer, buffer_size, "%02u:%02u:%02u", hours, minutes, seconds);
}