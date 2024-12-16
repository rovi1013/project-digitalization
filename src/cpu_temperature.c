//
// Created by vincent on 12/12/24.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tgmath.h>

#include "saul_reg.h"
#include "saul.h"
#include "ztimer.h"
#include "cpu_temperature.h"
#include "utils/timestamp_convert.h"
#include "utils/error_handler.h"

// Custom strdup implementation
char *my_strdup(const char *src) {
    if (src == NULL) {
        return NULL;
    }
    const size_t len = strlen(src) + 1; // +1 for null terminator
    char *dup = malloc(len);
    if (dup == NULL) {
        return NULL;
    }
    memcpy(dup, src, len);
    return dup;
}

// Execute CPU temperature action
cpu_temperature_t cpu_temperature_execute(void) {
    phydat_t data;
    cpu_temperature_t result = {
        .temperature = 0,
        .scale = 0,
        .device_name = NULL,
        .timestamp = 0,
        .status = ERROR_UNKNOWN
    };

    saul_reg_t *dev = saul_reg_find_type(SAUL_SENSE_TEMP);

    // Check device correctness
    if (dev == NULL) {
        result.status = ERROR_NO_SENSOR;
        return result;
    }
    result.device_name = my_strdup(dev->name);

    // Check correct device name allocation
    if (result.device_name == NULL) {
        result.status = ERROR_MEMORY_FAIL;
        return result;
    }

    // Read data from device
    result.timestamp = ztimer_now(ZTIMER_USEC);
    if (saul_reg_read(dev, &data) < 0) {
        result.status = ERROR_READ_FAIL;
        return result;
    }

    result.temperature = data.val[0];
    result.scale = data.scale;
    result.status = 0;

    return result;
}

void cpu_temperature_print(const cpu_temperature_t *temp) {
    char time_str[9];
    format_timestamp(temp->timestamp, time_str, sizeof(time_str));

    if (temp->status == 0) {
        // Scale the temperature value
        const double scaled_temp = temp->temperature * pow(10, temp->scale);

        // Print temperature and device info
        printf("[%s] The temperature of %s is %.2f °C\n",
               time_str, temp->device_name, scaled_temp);
    } else {
        const char *error_message = get_error_message(temp->status);
        printf("[%s] Error: %s (Device: %s)\n",
               time_str, error_message, temp->device_name ? temp->device_name : "Unknown");
    }
}

// Free memory and prevent dangling pointers
void cpu_temperature_cleanup(cpu_temperature_t *temp) {
    if (temp->device_name != NULL) {
        free(temp->device_name);
        temp->device_name = NULL;
    }
}

// Initialize CPU temperature sensor
void cpu_temperature_init(void) {
    puts("CPU temperature sensor initialized");
}

