//
// Created by vincent on 11/1/24.
//

#include <stdio.h>
#include <ztimer.h>

#include "msg.h"

#include "led_control.h"
#include "cpu_temperature.h"
#include "cmd_control.h"
#include "coap_post.h"
#include "thread.h"
#include "utils/error_handler.h"

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

char rcv_thread_stack[THREAD_STACKSIZE_MAIN];

void *coap_thread(void *arg) {
    (void) arg;

    while (true) {
        cpu_temperature_t temp;
        cpu_temperature_get(&temp);

        coap_request_t request;
        init_coap_request(&request);
        const int res = coap_post_send(&request, temp.temperature);

        handle_error(__func__, res);

        ztimer_sleep(ZTIMER_SEC, 60);
    }

    return NULL;
}

void *console_thread(void *arg) {
    cmd_control_init();
    return NULL;
}
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

    thread_create(rcv_thread_stack, sizeof(rcv_thread_stack),
        THREAD_PRIORITY_MAIN -2, 0,
        coap_thread, NULL, "coap_thread");

    //thread_2:
    #ifdef CONSOLE_USE {
        thread_create(rcv_thread_stack, sizeof(rcv_thread_stack),
            THREAD_PRIORITY_MAIN -1, 0,
            console_thread, NULL, "console_thread");
    }

    #endif

    return 0;
}