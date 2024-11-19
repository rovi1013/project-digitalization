# Project Digitalization (WiSe 2024/35)

This is the repository for the digitalization project at the FRA-UAS. Using RIOT-OS to create a small application to 
control a _Nordic_ nRF52840 (DK) device. Providing additional remote access via the _Telegram_ bot API. 

SAUL (Sensor Actuator Uber Layer) API ist wohl wichtig


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
│   ├── cmd_control.h             # Shell control header
│   └── further classes           # ...
├── CMakeLists.txt                # CMake project file
└── README.md                     # Documentation

```

### Main Class

Entry point of the application that initializes all modules and starts the main event loop.

### Class led_control

Handles direct interaction with the board’s LEDs.

### Class shell_control

Used to control the board via the shell, useful for testing.

### Class telegram_bot

Manages communication with the _Telegram_ API.

### Class ...

...


## RIOT-OS Modules

### [periph/gpio.h](https://doc.riot-os.org/group__drivers__periph__gpio.html)

Peripheral driver to control the specific pins on the board.

Used in: led_control

### [board.h](https://doc.riot-os.org/group__boards__nrf52840dk.html)

Specific configuration for the nRF52840 DK board, used to address the LED pins.

Used in: led_control

### [shell.h](https://doc.riot-os.org/group__sys__shell.html)

Simple shell interpreter used to parse arguments.

### [sock.h](https://doc.riot-os.org/group__net__sock__udp.html)

A network API for applications and libraries, used to create custom HTTP functionality with the UDP submodule.

### [jsmn.h](https://doc.riot-os.org/group__pkg__jsmn.html)

JSON parser library, used to process _Telegram_ bot messages.

### SAUL Driver: [DHT Family of Humidity and Temperature Sensors](https://doc.riot-os.org/group__drivers__dht.html)
Temperature and humidity sensor.


## Temperature Sensor (DHT20)
TODO: Explain (short)

### PINs
| DHT20 Pin | Function       | nRF52840-DK Pin           |
|-----------|----------------|---------------------------|
| VCC       | Power (3.3V)   | 3V3 on nRF52840-DK        |
| GND       | Ground         | GND                       |
| SDA       | I2C Data Line  | GPIO pin with I2C support |
| SCL       | I2C Clock Line | GPIO pin with I2C support |




## Network Connectivity
The IoT device we are using in this project (nRF52840) has BLE (no WLAN or LAN) connectivity only, as these devices usually do.
Therefore, we have to use a border router which can connect to our device and to a "normal" network. For this we are
using a raspberry-pi and the nRF52840-Dongle.
These two can be seen as a router.
TODO: explain network structure.

IPv6 lowpan to connect BLE (Bluetooth low energy) of nrf board to standard ipv6, while saving a lot of size for the transmission (e.g. IPv6 header size).
gnrc_networking make all term for interface, use variable PORT (from makefile) to connect if instance already running
Connect to internet:
dist/tools/tapsetup/tapsetup -u <interface> ; use ethernet as interface to add this to the network

### Raspberry-Pi Setup

ip addr show or /sbin/ifconfig 

-> look up address range

sudo nmap -sn 192.168.0.1/24

= gives every device connected to this network

ssh to device -> authenticate with username and password

riot@6lbr-3

TODO: HOW TO SETUP RPI.

### nRF52840-Dongle Setup

nrfutil version >=6.1.1 required

-> requires Python >=3.7, <3.11 (https://pypi.org/project/nrfutil/)

Change Makefile in RIOT/examples/gnrc_border_router/Makefile according to
https://teaching.dahahm.de/teaching/ss23/project/2023/05/06/nrf52840dongle_6lbr.html
Use gnrc_border_router from AllRIOT/RIOT repository

make package (for nrf52840dongle)

find usb device (lsusb), maybe need to push reset button, find target usb port (/dev/ttyACM*, in our case /dev/ttyACM0)

create .zip (for flash): nrfutil pkg generate --hw-version 52 --sd-req 0x00 --application gnrc_border_router.hex --application-version 1 gnrc_border_router.hex.zip
(explain variables!)

flash the device with .zip: nrfutil dfu usb-serial --port /dev/ttyACM0 --package gnrc_border_router.hex.zip

Test with: ...



## _Telegram_ Bot Integration

### Set Up a Bot

1. Open _Telegram_
2. Search for "BotFather" and start a new chat with it
3. Use ``/newbot`` to create a new bot
4. Use the provided token to access the HTTP API

The bot "BotFather" is used to create and manage bot accounts.

_Telegram_ Bot API [Documentation](https://core.telegram.org/bots/api)

### Bot Description

This bot is used to send and receive messages to and from a small (proof-of-concept) IoT device. These messages can be 
used to control the device running on RIOT-OS. See "Commands" for a list of all available controls.

### Bot Commands

Turn on an LED:
```bash
led <1-4> <1-4> ... on
```

Turn off an LED:
```bash
led <1-4> <1-4> ... off
```

Toggle an LED:
```bash
led <1-4> <1-4> ... toggle
```

You can control the LEDs by their associated number written on the board (LED1, LED2, LED3, LED4 -> <1-4>).
Inferentially the maximum number of target LEDs for the commands above is 4.


## Bash Control

Alternatively, mainly for testing purposes, you can control the LEDs directly via the RIOT-OS command line.

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

The rules for these commands are the same as those for the _Telegram_ bot.


## Useful commands

Flash the nRF52840 board:

```bash
BOARD=nrf52840dk make all flash term
```

List all available RIOT modules:

```bash
make info-modules
```


## Ask the Prof

### Module ``shell_commands``

[Makefile](src/Makefile) module ``shell_commands`` error:

```bash
Error - using unknown modules: shell_commands
make: *** [/home/vincent/Workspace/project-digitalization/RIOT//Makefile.include:742: ..module-check] Error 1
```

Despite ``make info-modules`` showing that ``shell_commands`` is available. And the module is working in ``Tutorials/task-01`` on the same machine.

**Answer**: Wrong name, correct name is `shell_cmds_default`

### Use of "jsmn" library

The jsmn library (parse JSON) is not included in RIOT OS despite it being mentioned in the [documentation](https://doc.riot-os.org/group__pkg__jsmn.html).

**Answer**: Not a module but a package. Use `USEPKG` instead of `USEMODULE`. Refer to RIOT/tests/ for example 
implementations of most features (modules and packages) for the correct and up-to-date implementation.

### Makefile location

Is there any way to use the project structure where the Makefile is not in the same directory as the main.c file?


## TODOs

- [ ] Which instant messaging protocol should we use?
- [ ] Which modules from RIOT-OS do we need?
- [ ] Create the project architecture.
- [ ] Develop the application.
- [ ] Present the project.


## Timeline

### 2024-11-25: Architecture

- Submission and presentation of your architecture

### 2025-01-20: Demo

- Present your walking skeleton (incl. demo)

### 2025-02-10: Presentation

- Give a short presentation on your work (live demo?)

### 2025-02-21: Submission

- Final version of the code is in the repository
- You have granted access to me
- Send me your documentation
