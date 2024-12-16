//
// Created by vincent on 12/12/24.
//

#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "saul_reg.h"
#include "saul.h"
#include "ztimer.h"
#include "cpu_temperature.h"
#include "utils/timestamp_convert.h"
#include "utils/error_handler.h"

// // Custom strdup implementation
// char *my_strdup(const char *src) {
//     const size_t len = strlen(src) + 1;     // String plus '\0'
//     char *dst = malloc(len);                // Allocate space
//     if (dst == NULL) return NULL;           // No memory
//     memcpy (dst, src, len);                 // Copy the block
//     return dst;                             // Return the new string
// }

// Execute CPU temperature action
cpu_temperature_t cpu_temperature_execute(void) {
    phydat_t data;
    cpu_temperature_t cpu_temp = {
        .temperature = 0,
        .scale = 0,
        .device_name = NULL,
        .timestamp = 0,
        .status = ERROR_UNKNOWN
    };

    saul_reg_t *device = saul_reg_find_type(SAUL_SENSE_TEMP);

#ifdef BOARD_NATIVE
    // Ignore unused variables
    (void) data;
    (void) device;
    // Mock temperature data for the native platform
    cpu_temp.temperature = 2500; // Mock value (25.00°C)
    cpu_temp.scale = -2;
    cpu_temp.device_name = strdup("Mock-Temperature-Sensor");
    cpu_temp.timestamp = 0;
    cpu_temp.status = 0;
#else

    // Check device correctness
    if (device == NULL) {
        cpu_temp.status = ERROR_NO_SENSOR;
        return cpu_temp;
    }
    cpu_temp.device_name = strdup(device->name);

    // Check correct device name allocation
    if (cpu_temp.device_name == NULL) {
        cpu_temp.status = ERROR_MEMORY_FAIL;
        return cpu_temp;
    }

    // Read data from device
    cpu_temp.timestamp = ztimer_now(ZTIMER_USEC);
    if (saul_reg_read(device, &data) < 0) {
        cpu_temp.status = ERROR_READ_FAIL;
        return cpu_temp;
    }

    cpu_temp.temperature = data.val[0];
    cpu_temp.scale = data.scale;
    cpu_temp.status = 0;

#endif

    return cpu_temp;
}

// Small helper function for circumvent using a (complex) math library
double scale_factor(const int8_t scale) {
    static const double scale_factors[] = {0.1, 0.01, 0.001};
    if (scale == -1) { return scale_factors[0]; }
    if (scale == -2) { return scale_factors[1]; }
    if (scale == -3) { return scale_factors[2]; }
    return 1.0;
}

void cpu_temperature_print(const cpu_temperature_t *temp) {
    char time_str[9];
    format_timestamp(temp->timestamp, time_str, sizeof(time_str));

    if (temp->status == 0) {
        // Scale the temperature value
        const double scaled_temp = temp->temperature * scale_factor(temp->scale);

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

