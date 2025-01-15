//
// Created by vincent on 12/12/24.
//

#include <string.h>
#include <stdio.h>

#include "saul_reg.h"
#include "saul.h"
#include "ztimer.h"
#include "cpu_temperature.h"
#include "utils/timestamp_convert.h"
#include "utils/error_handler.h"

// Execute CPU temperature action
cpu_temperature_t cpu_temperature_execute(void) {
    phydat_t data;
    cpu_temperature_t cpu_temp = {
        .temperature = 0,
        .scale = 0,
        .device_name = "Unknown",
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
    snprintf(cpu_temp.device_name, DEVICE_NAME_MAX_LEN, "%s", "Mock-Temperature-Sensor");
    cpu_temp.timestamp = 0;
    cpu_temp.status = 0;
#else

    // Check device correctness
    if (device == NULL) {
        cpu_temp.status = ERROR_NO_SENSOR;
        return cpu_temp;
    }
    snprintf(cpu_temp.device_name, DEVICE_NAME_MAX_LEN, "%s", device->name);

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

// Small helper function to determine the divisor
int determine_divisor(const int8_t scale) {
    int divisor = 1;
    if (scale == -1) { divisor = 10; }
    else if (scale == -2) { divisor = 100; }
    else if (scale == -3) { divisor = 1000; }
    return divisor;
}

// Print the CPU temperature
void cpu_temperature_print(const cpu_temperature_t *temp) {
    char time_str[9];
    format_timestamp(temp->timestamp, time_str, sizeof(time_str));

    // Calculate the Integer and Factorial part of the scaled temperature
    const int divisor = determine_divisor(temp->scale);
    const int integer_part = temp->temperature / divisor;
    const int fractional_part = temp->temperature % divisor;

    if (temp->status == 0) {
        // Print temperature and device info
        printf("[%s] The temperature of %s is %d.%0*d °C\n",
               time_str, temp->device_name, integer_part, (temp->scale < 0 ? -temp->scale : 0), fractional_part);
    } else {
        // Handle errors
        const char *error_message = get_error_message(temp->status);
        printf("[%s] Error: %s (Device: %s)\n",
               time_str, error_message, temp->device_name);
    }
}

// Initialize CPU temperature sensor
void cpu_temperature_init(void) {
    puts("CPU temperature sensor initialized");
}
