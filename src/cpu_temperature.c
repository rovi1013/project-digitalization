//
// Created by vincent on 12/12/24.
//

#include <stdio.h>
#include <stdlib.h>

#include "saul_reg.h"
#include "saul.h"
#include "cpu_temperature.h"


// Execute CPU temperature action
int cpu_temperature_execute(void) {
    phydat_t data;
    saul_reg_t *dev = saul_reg_find_type(SAUL_SENSE_TEMP);
    if (!dev) {
        printf("No CPU temperature sensor found\n");
        return -1;
    }

    if (saul_reg_read(dev, &data) < 0) {
        printf("Failed to read CPU temperature\n");
        return -1;
    }

    printf("CPU temperature: %d.%02d°C\n", data.val[0] / 100, abs(data.val[0] % 100));
    return 0;
}

// Initialize CPU temperature sensor
void cpu_temperature_init(void) {
    printf("CPU temperature sensor initialized\n");
}

