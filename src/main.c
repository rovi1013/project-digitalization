//
// Created by vincent on 11/1/24.
//

#include <stdio.h>
#include <stdlib.h>
#include <ztimer.h>

#include "msg.h"
#include "thread.h"

#include "led_control.h"
#include "cmd_control.h"
#include "configuration.h"
#include "cpu_temperature.h"
#include "coap_post.h"
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
static msg_t cmd_msg_queue[MAIN_QUEUE_SIZE];
char console_thread_stack[THREAD_STACK_SIZE];
#endif

void *coap_thread(void *arg) {
    (void) arg;
    msg_init_queue(coap_msg_queue, MAIN_QUEUE_SIZE);

    while (1) {
        const uint32_t start_time = ztimer_now(ZTIMER_MSEC);

        char buffer_temp[CLASS_COAP_BUFFER_SIZE];
        cpu_temperature_t temp;
        cpu_temperature_get(&temp);
        cpu_temperature_formatter(&temp, CALL_FROM_CLASS_COAP, buffer_temp, CLASS_COAP_BUFFER_SIZE);

        const int res = coap_post_send(buffer_temp, "all");
        handle_error(__func__, res);

        if (res == COAP_SUCCESS) {
            if (app_config.enable_led_feedback) {
                led_control_execute(0, "on");
            }
            uint32_t wait_time = 1000;  // Max wait time
            while (!get_coap_response_status() && wait_time > 0) {
                ztimer_sleep(ZTIMER_MSEC, 50);
                wait_time -= 50;
            }
        }
        uint32_t elapsed_time = ztimer_now(ZTIMER_MSEC) - start_time;  // Compute elapsed time
        printf("CoAP communication took %lu ms to finish.\n", (unsigned long)elapsed_time);

        if (app_config.enable_led_feedback) {
            led_control_execute(0, "off");
        }

        ztimer_sleep(ZTIMER_MSEC, config_get_notification_interval() * 60000);
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

    // Initialize the configuration
    config_init();

    msg_init_queue(main_msg_queue, MAIN_QUEUE_SIZE);

    // Thread #1: CoAP
    thread_create(coap_thread_stack, THREAD_STACK_SIZE,
        6, THREAD_CREATE_STACKTEST, coap_thread, NULL, "CoapThread");

    // Thread #2: Console
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