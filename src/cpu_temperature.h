//
// Created by vincent on 12/12/24.
//

#ifndef CPU_TEMPERATURE_H
#define CPU_TEMPERATURE_H

#define DEVICE_NAME_MAX_LEN 32

# define CLASS_CMD_BUFFER_SIZE 80
# define CLASS_COAP_BUFFER_SIZE 30

/**
 * Store the cpu temperature and more meta information
 */
typedef struct {
    int16_t temperature;                        /**< CPU temperature reading */
    int8_t scale;                               /**< Scale of measurement (10^scale) */
    char device_name[DEVICE_NAME_MAX_LEN];      /**< Device name */
    uint32_t timestamp;                         /**< Time of the reading in microseconds */
    int8_t status;                              /**< Custom codes defined in error_handler.h */
} cpu_temperature_t;

/**
 * Differentiate between different caller classes
 */
typedef enum {
    CALL_FROM_CLASS_CMD,
    CALL_FROM_CLASS_COAP
} caller_class_t;

/**
 * Read CPU temperature
 * @param cpu_temp Pointer to a cpu_temperature_t struct
 * @return A cpu_temperature_t struct
 */
int cpu_temperature_get(cpu_temperature_t *cpu_temp);

/**
 * Formats the CPU temperature struct into a 'nice' string depending on the caller class
 * @param cpu_temp Pointer to a cpu_temperature_t struct
 * @param caller_class Name of the caller class
 * @param buffer Pointer to the formatted output string
 * @param buffer_size Size of the output string
 */
void cpu_temperature_formatter(const cpu_temperature_t *cpu_temp, caller_class_t caller_class, char *buffer, size_t buffer_size);

#endif //CPU_TEMPERATURE_H
