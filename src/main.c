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

#ifdef BOARD_NATIVE
#define THREAD_STACK_SIZE (4096)
#else
#define THREAD_STACK_SIZE (2048)
#endif

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

char coap_thread_stack[THREAD_STACK_SIZE];
#if ENABLE_CONSOLE_THREAD == 1
char console_thread_stack[THREAD_STACK_SIZE];
#endif

void *coap_thread(void *arg) {
    (void) arg;
    while (1) {
        char buffer_temp[CLASS_COAP_BUFFER_SIZE];
        cpu_temperature_t temp;
        cpu_temperature_get(&temp);
        cpu_temperature_formatter(&temp, CALL_FROM_CLASS_COAP, buffer_temp, CLASS_COAP_BUFFER_SIZE);

        coap_request_t request;
        init_coap_request(&request);
        const int res = coap_post_send(&request, buffer_temp);

        handle_error(__func__, res);

        ztimer_sleep(ZTIMER_MSEC, 60000);
    }
    return NULL;
}

#if ENABLE_CONSOLE_THREAD == 1
void *console_thread(void *arg) {
    (void) arg;
    cmd_control_init();
    return NULL;
}
#endif

int main(void) {
    // Initialize devices
    led_control_init();
    cpu_temperature_init();

    // Initialize thread message queue
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    // Thread #1
    thread_create(coap_thread_stack, THREAD_STACK_SIZE,
        7, THREAD_CREATE_STACKTEST, coap_thread, NULL, "CoapThread");

    // Thread #2:
#if ENABLE_CONSOLE_THREAD == 1
    thread_create(console_thread_stack, THREAD_STACK_SIZE,
        7, THREAD_CREATE_STACKTEST, console_thread, NULL, "ConsoleThread");
#endif

    return 0;
}