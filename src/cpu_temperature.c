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

// Map the temperature unit enumerators to strings
const unit_map_t temperature_unit_map[] = {
    {UNIT_UNDEF, "undefined"},
    {UNIT_NONE, "none"},
    {UNIT_TEMP_C, "°C"},
    {UNIT_TEMP_F, "°F"},
    {UNIT_TEMP_K, "°K"}
};

// Use the unit map to get the unit string
const char *unit_to_string(const uint8_t unit) {
    for (size_t i = 0; i < sizeof(temperature_unit_map) / sizeof(temperature_unit_map[0]); i++) {
        if (temperature_unit_map[i].unit == unit) {
            return temperature_unit_map[i].unit_string;
        }
    }
    return "undefined";
}

// Execute CPU temperature action
int cpu_temperature_get(cpu_temperature_t *cpu_temp) {
    if (!cpu_temp) { // Null pointer
        handle_error(__func__,ERROR_NULL_POINTER);
        return ERROR_NULL_POINTER;
    }
    cpu_temp->temperature = 0;
    cpu_temp->scale = 0;
    cpu_temp->unit = 0;
    snprintf(cpu_temp->device_name, DEVICE_NAME_MAX_LEN, "%s", "Unknown");
    cpu_temp->timestamp = 0;
    cpu_temp->status = ERROR_UNKNOWN;

#ifdef BOARD_NATIVE
    // Mock temperature data for the native platform
    cpu_temp->temperature = 2500; // Mock values for 25.00°C
    cpu_temp->scale = -2;
    cpu_temp->unit = UNIT_TEMP_C;
    snprintf(cpu_temp->device_name, DEVICE_NAME_MAX_LEN, "%s", "Mock-Sensor");
    cpu_temp->timestamp = 0;
    cpu_temp->status = 0;
#else
    phydat_t data;
    saul_reg_t *device = saul_reg_find_type(SAUL_SENSE_TEMP);

    saul_reg_t *device = saul_reg_find_type(SAUL_SENSE_TEMP);

    // Check device correctness
    if (device == NULL) {
        cpu_temp->status = ERROR_NO_SENSOR;
        handle_error(__func__,ERROR_NO_SENSOR);
        return ERROR_NO_SENSOR;
    }

    snprintf(cpu_temp->device_name, DEVICE_NAME_MAX_LEN, "%s", device->name);

    // Read data from a device
    cpu_temp->timestamp = ztimer_now(ZTIMER_USEC);
    if (saul_reg_read(device, &data) < 0) {
        cpu_temp->status = ERROR_TEMP_READ_FAIL;
        handle_error(__func__,ERROR_TEMP_READ_FAIL);
        return ERROR_TEMP_READ_FAIL;
    }

    cpu_temp->temperature = data.val[0];
    cpu_temp->scale = data.scale;
    cpu_temp->unit = data.unit;
    cpu_temp->status = 0;

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
void cpu_temperature_formatter(const cpu_temperature_t *cpu_temp, const caller_class_t caller_class, char *buffer, const size_t buffer_size) {
    char time_str[9];
    format_timestamp(cpu_temp->timestamp, time_str, sizeof(time_str));

    // Calculate the Integer and Factorial part of the scaled temperature
    const int divisor = determine_divisor(cpu_temp->scale);
    const int integer_part = cpu_temp->temperature / divisor;
    const int fractional_part = cpu_temp->temperature % divisor;

    // Use 'CPU' as device name instead of NRF_TEMP in the case of nRF devices
    char device_name[DEVICE_NAME_MAX_LEN];
    if (strcmp(cpu_temp->device_name, "NRF_TEMP") == 0) {
        snprintf(device_name, DEVICE_NAME_MAX_LEN, "%s", "CPU");
    } else {
        snprintf(device_name, DEVICE_NAME_MAX_LEN, "%s", cpu_temp->device_name);
    }

    if (cpu_temp->status == 0) {
        switch (caller_class) {
            case CALL_FROM_CLASS_CMD:
                // Print temperature and device info
                snprintf(buffer, buffer_size, "[%s] The temperature of %s is %d.%0*d %s.\nRaw phydat_t data: temp: %d, scale: %d, unit: %d.\n",
                        time_str, device_name, integer_part, (cpu_temp->scale < 0 ? -cpu_temp->scale : 0),
                        fractional_part, unit_to_string(cpu_temp->unit),
                        cpu_temp->temperature, cpu_temp->scale, cpu_temp->unit);
                break;
            case CALL_FROM_CLASS_COAP:
                // Print temperature only
                snprintf(buffer, buffer_size, "%s Temperature: %d.%0*d %s\n",
                        device_name, integer_part, (cpu_temp->scale < 0 ? -cpu_temp->scale : 0),
                        fractional_part, unit_to_string(cpu_temp->unit));
                break;
            default:
                handle_error(__func__, ERROR_CALLER_UNKNOWN);
        }
    } else {
        handle_error(__func__,cpu_temp->status);
    }
}
