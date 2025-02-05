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
        printf("Ich fange an\n");
        cpu_temperature_t temp;
        cpu_temperature_get(&temp);

        char str[10];
        sprintf(str, "%d", temp.temperature);

        coap_request_t request;
        init_coap_request(&request);
        const int res = coap_post_send(&request, str);

        handle_error(__func__, res);

        printf("Ich schlafe jetzt\n");
        ztimer_sleep(ZTIMER_SEC, 60);
        printf("ich bin aufgewacht\n");
    }

    return NULL;
}

void *console_thread(void *arg) {
    (void) arg;
    cmd_control_init();
    return NULL;
}
int main(void) {
    // Initialize devices
    led_control_init();
    cpu_temperature_init();

    // Initialize default networking
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    // Thread #1 {
    thread_create(rcv_thread_stack, sizeof(rcv_thread_stack),
        THREAD_PRIORITY_MAIN -1, 0,
        coap_thread, NULL, "coap_thread");


    // Thread #2:
    thread_create(rcv_thread_stack, sizeof(rcv_thread_stack),
        THREAD_PRIORITY_MAIN -1, 0,
        console_thread, NULL, "console_thread");

    // Keep both threads running
    while (1) {
        thread_yield();
    }

    return 0;
}