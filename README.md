# Project Digitalization (WiSe 2024/35) - Demo Application

This is a simple demo application for the digitalization project at the FRA-UAS. Using RIOT-OS to create a small application to 
control a _Nordic_ nRF52840 (DK) device. Using the shell to control the LEDs on this IoT board. 


## Project Structure

```bash
project/digitalization
├── Makefile                      # Wrapper Makefile
├── .env                          # Secrets storage file
├── src/
│   ├── Makefile                  # Main Makefile
│   ├── main.c                    # Main application file
│   ├── led_control.c             # LED control source
│   ├── led_control.h             # LED control header
│   ├── cmd_control.c             # Shell control source
│   └── cmd_control.h             # Shell control header
├── CMakeLists.txt                # CMake project file
└── README.md                     # Documentation

```

### Main Class

Entry point of the application that initializes all modules and starts the main event loop.

### Class led_control

Handles direct interaction with the board’s LEDs.

### Class shell_control

Used to control the board via the shell.


## RIOT-OS Modules

### [periph/gpio.h](https://doc.riot-os.org/group__drivers__periph__gpio.html)

Peripheral driver to control the specific pins on the board.

Used in: led_control

### [board.h](https://doc.riot-os.org/group__boards__nrf52840dk.html)

Specific configuration for the nRF52840 DK board, used to address the LED pins.

Used in: led_control

### [shell.h](https://doc.riot-os.org/group__sys__shell.html)

Simple shell interpreter used to parse arguments.


## Bash Control

You can control the LEDs directly via the RIOT-OS command line.

Turn on an LED:
```bash
led_on <1-4> <1-4> ...
```

Turn off an LED:
```bash
led_off <1-4> <1-4> ...
```

Toggle an LED:
```bash
led_toggle <1-4> <1-4> ...
```

You can control the LEDs by their associated number written on the board (LED1, LED2, LED3, LED4 -> <1-4>).
Inferentially the maximum number of target LEDs for the commands above is 4.


## Useful commands

Flash the nRF52840 board:

```bash
BOARD=nrf52840dk make all flash term
```

List all available RIOT modules:

```bash
make info-modules
```
