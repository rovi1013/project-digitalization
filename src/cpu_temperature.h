//
// Created by vincent on 12/12/24.
//

#ifndef CPU_TEMPERATURE_H
#define CPU_TEMPERATURE_H

#define DEVICE_NAME_MAX_LEN = 32;

typedef struct {
    int16_t temperature;    // Temperature reading
    int8_t scale;           // Scale of measurement (10^scale)
    char *device_name;      // Device name
    uint32_t timestamp;     // Time of the reading in microseconds
    int8_t status;          // Status: 0 for success; negative for errors
} cpu_temperature_t;

// Read CPU temperature
cpu_temperature_t cpu_temperature_execute(void);

// Print the CPU temperature
void cpu_temperature_print(const cpu_temperature_t *temp);

// Free memory and prevent dangling pointers
void cpu_temperature_cleanup(cpu_temperature_t *temp);

// Initialize CPU temperature sensor
void cpu_temperature_init(void);

#endif //CPU_TEMPERATURE_H
