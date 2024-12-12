//
// Created by vincent on 12/7/24.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "sensor_mock.h"

// Generate random mock data
static int16_t generate_mock_data(const char *type) {
    if (strcmp(type, "temp") == 0) {
        return rand() % 51; // Random temperature between 0 and 50
    }
    if (strcmp(type, "hum") == 0) {
        return rand() % 101; // Random humidity between 0 and 100
    }

    return -1; // Unsupported type
}

// Execute mock sensor actions
int sensor_mock_execute(const char *action) {
    const int16_t data = generate_mock_data(action);

    if (data < 0) {
        printf("Unknown action '%s' for mock sensor\n", action);
        return -1;
    }

    if (strcmp(action, "temp") == 0) {
        printf("Mock temperature: %d°C\n", data);
    } else if (strcmp(action, "hum") == 0) {
        printf("Mock humidity: %d%%\n", data);
    }

    return 0;
}

// Initialize mock sensor
void sensor_mock_init(void) {
    printf("Mock temperature and humidity sensor initialized\n");
}


