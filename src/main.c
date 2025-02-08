//
// Created by vincent on 11/1/24.
//

#include <stdio.h>
#include <stdlib.h>
#include <ztimer.h>

#include "msg.h"

#include "led_control.h"
#include "cpu_temperature.h"
#include "cmd_control.h"
#include "coap_post.h"
#include "thread.h"
#include "utils/error_handler.h"

#ifdef BOARD_NATIVE
#define THREAD_STACK_SIZE (4096)
#else
#define THREAD_STACK_SIZE (2048)
#endif

#define MAIN_QUEUE_SIZE     (16)
static msg_t main_msg_queue[MAIN_QUEUE_SIZE];
static msg_t coap_msg_queue[MAIN_QUEUE_SIZE];

char coap_thread_stack[THREAD_STACK_SIZE];
#if ENABLE_CONSOLE_THREAD == 1
char console_thread_stack[THREAD_STACK_SIZE];
static msg_t cmd_msg_queue[MAIN_QUEUE_SIZE];
#endif

void *coap_thread(void *arg) {
    (void) arg;
    msg_init_queue(coap_msg_queue, MAIN_QUEUE_SIZE);


    while (1) {
        uint32_t start_time = ztimer_now(ZTIMER_MSEC);

        char buffer_temp[CLASS_COAP_BUFFER_SIZE];
        cpu_temperature_t temp;
        cpu_temperature_get(&temp);
        cpu_temperature_formatter(&temp, CALL_FROM_CLASS_COAP, buffer_temp, CLASS_COAP_BUFFER_SIZE);

        coap_request_t request;
        init_coap_request(&request);
        coap_response_status = false;
        const int res = coap_post_send(&request, buffer_temp);

        handle_error(__func__, res);

        if (res == COAP_SUCCESS) {
            led_control_execute(0,"on");
            uint32_t wait_time = 5000;  // Max wait time
            while (!coap_response_status && wait_time > 0) {
                ztimer_sleep(ZTIMER_MSEC, 100);
                wait_time -= 100;
            }

            led_control_execute(0,"off");
        }
        uint32_t elapsed_time = ztimer_now(ZTIMER_MSEC) - start_time;  // Compute elapsed time
        printf("CoAP thread running. Elapsed time: %lu ms\n", (unsigned long)elapsed_time);

        ztimer_sleep(ZTIMER_MSEC, 1000);
    }
    return NULL;
}

#if ENABLE_CONSOLE_THREAD == 1
void *console_thread(void *arg) {
    (void) arg;
    msg_init_queue(cmd_msg_queue, MAIN_QUEUE_SIZE);
    cmd_control_init();
    return NULL;
}
#endif

int main(void) {
    // Wait 1 second to allow for everything to load correctly
    ztimer_sleep(ZTIMER_MSEC, 1000);
    // Initialize devices
    led_control_init();
    cpu_temperature_init();
    msg_init_queue(main_msg_queue, MAIN_QUEUE_SIZE);

    // Thread #1
    thread_create(coap_thread_stack, THREAD_STACK_SIZE,
        7, THREAD_CREATE_STACKTEST, coap_thread, NULL, "CoapThread");

    // Thread #2:
#if ENABLE_CONSOLE_THREAD == 1
    thread_create(console_thread_stack, THREAD_STACK_SIZE,
        7, THREAD_CREATE_STACKTEST, console_thread, NULL, "ConsoleThread");
#endif

    msg_t msg;
    while (1) {
        msg_receive(&msg);
    }

    return 0;
}