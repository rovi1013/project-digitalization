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
int cpu_temperature_get(cpu_temperature_t *cpu_temperature) {
    if (!cpu_temperature) { // Null pointer
        handle_error(__func__,ERROR_NULL_POINTER);
        return ERROR_NULL_POINTER;
    }
    phydat_t data;
    cpu_temperature->temperature = 0;
    cpu_temperature->scale = 0;
    snprintf(cpu_temperature->device_name, DEVICE_NAME_MAX_LEN, "%s", "Unknown");
    cpu_temperature->timestamp = 0;
    cpu_temperature->status = ERROR_UNKNOWN;

#ifdef BOARD_NATIVE
    // Ignore unused variables
    (void) data;
    // Mock temperature data for the native platform
    cpu_temperature->temperature = 2500; // Mock value (25.00°C)
    cpu_temperature->scale = -2;
    snprintf(cpu_temperature->device_name, DEVICE_NAME_MAX_LEN, "%s", "Mock-Temperature-Sensor");
    cpu_temperature->timestamp = 0;
    cpu_temperature->status = 0;
#else

    saul_reg_t *device = saul_reg_find_type(SAUL_SENSE_TEMP);

    // Check device correctness
    if (device == NULL) {
        cpu_temperature->status = ERROR_NO_SENSOR;
        handle_error(__func__,ERROR_NO_SENSOR);
        return TEMP_SUCCESS;
    }
    snprintf(cpu_temperature->device_name, DEVICE_NAME_MAX_LEN, "%s", device->name);

    // Read data from device
    cpu_temperature->timestamp = ztimer_now(ZTIMER_USEC);
    if (saul_reg_read(device, &data) < 0) {
        cpu_temperature->status = ERROR_TEMP_READ_FAIL;
        handle_error(__func__,ERROR_TEMP_READ_FAIL);
        return TEMP_SUCCESS;
    }

    cpu_temperature->temperature = data.val[0];
    cpu_temperature->scale = data.scale;
    cpu_temperature->status = 0;

#endif

    return TEMP_SUCCESS;
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
void cpu_temperature_formatter(const cpu_temperature_t *temp, const caller_class_t caller_class, char *buffer, const size_t buffer_size) {
    char time_str[9];
    format_timestamp(temp->timestamp, time_str, sizeof(time_str));

    // Calculate the Integer and Factorial part of the scaled temperature
    const int divisor = determine_divisor(temp->scale);
    const int integer_part = temp->temperature / divisor;
    const int fractional_part = temp->temperature % divisor;

    if (temp->status == 0) {
        switch (caller_class) {
            case CALL_FROM_CLASS_CMD:
                // Print temperature and device info
                snprintf(buffer, buffer_size, "[%s] The temperature of %s is %d.%0*d °C\n",
                        time_str, temp->device_name, integer_part,
                        (temp->scale < 0 ? -temp->scale : 0), fractional_part);
                break;
            case CALL_FROM_CLASS_COAP:
                // Print temperature only
                snprintf(buffer, buffer_size, "CPU Temperature: %d.%0*d °C\n",
                        integer_part, (temp->scale < 0 ? -temp->scale : 0), fractional_part);
                break;
            default:
                handle_error(__func__, ERROR_CALLER_UNKNOWN);
        }
    } else {
        handle_error(__func__,temp->status);
    }
}

// Initialize CPU temperature sensor
void cpu_temperature_init(void) {
    puts("CPU temperature sensor initialized");
}
