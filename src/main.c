//
// Created by vincent on 11/1/24.
//

#include "led_control.h"
#include "cpu_temperature.h"
#include "sensor_mock.h"
#include "cmd_control.h"

int main(void) {
    // Initialize devices
    led_control_init();
    cpu_temperature_init();
    sensor_mock_init();

    // Start the shell
    cmd_control_init();

    return 0;
}