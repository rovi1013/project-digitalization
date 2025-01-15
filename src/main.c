//
// Created by vincent on 11/1/24.
//

#include "msg.h"

#include "led_control.h"
#include "cpu_temperature.h"
#include "cmd_control.h"

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

int main(void) {
    // Initialize devices
    led_control_init();
    cpu_temperature_init();

    // Initialize default networking
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    // Start the shell
    cmd_control_init();

    return 0;
}