//
// Created by vincent on 11/1/24.
//

#include "msg.h"

#include "led_control.h"
#include "cpu_temperature.h"
#include "cmd_control.h"
#include "coap_post.h"

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

int main(void) {
    // Initialize devices
    led_control_init();
    cpu_temperature_init();
    // Initialize default networking
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    //thread_1 {
        // init coap struct with env variables: init_coap_request()
        // call cpu_temp
        // write cpu_temp to coap struct in .data
        // call coap post method with struct
    //}

    //thread_2:
    cmd_control_init();

    return 0;
}