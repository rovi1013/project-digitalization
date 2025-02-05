//
// Created by vincent on 12/12/24.
//

#ifndef CPU_TEMPERATURE_H
#define CPU_TEMPERATURE_H

#define DEVICE_NAME_MAX_LEN 32

typedef struct {
    int16_t temperature;                        /**< CPU temperature reading */
    int8_t scale;                               /**< Scale of measurement (10^scale) */
    char device_name[DEVICE_NAME_MAX_LEN];      /**< Device name */
    uint32_t timestamp;                         /**< Time of the reading in microseconds */
    int8_t status;                              /**< Custom codes defined in error_handler.h */
} cpu_temperature_t;

/**
 * Read CPU temperature
 * @param cpu_temperature Pointer to a cpu_temperature_t struct
 * @return A cpu_temperature_t struct
 */
int cpu_temperature_get(cpu_temperature_t *cpu_temperature);

/**
 * Print the CPU temperature to the console
 * @param temp Pointer to a cpu_temperature_t struct
 */
void cpu_temperature_print(const cpu_temperature_t *temp);

/**
 * Initialize CPU temperature
 */
void cpu_temperature_init(void);

#endif //CPU_TEMPERATURE_H
