//
// Created by vincent on 11/1/24.
//

#include <stdio.h>
#include "ztimer.h"

#include "led_control.h"
#include "cmd_control.h"

int main(void) {
    // Initialize components
    led_control_init();
    cmd_control_init();

    return 0;
}

