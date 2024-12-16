# Project Digitalization (WiSe 2024/35)

This is the repository for the digitalization project at the FRA-UAS. Using RIOT-OS to create a small application to 
control a _Nordic_ nRF52840 (DK) device. Providing additional remote access via the _Telegram_ bot API. 


## Usage

### Setup

1. Connect the nRF52840-DK board to Your PC and ensure the board is powered on (status LED).
2. Build and Flash the Application.

Navigate to the project directory:
```shell
cd project-digitalization
```

Build the application (this includes RIOT-OS) and open a terminal to the board:
```shell
make all clean build term
```

### Shell Commands

Control LEDs (brightness can be any value 0-255):
```shell
led <id> <on/off/brighness>
```

Read CPU temperature:
```shell
cpu-temp
```

Read mock sensor data:
```shell
mock <temp/hum>
```

Configure and list network interfaces:
```shell
ifconfig
```

List more commands:
```shell
help
```

### Network connectivity

To connect the nRF52840-DK board to the internet we need the [border router setup](#border-router-setup).

Currently, this only allows for communication between the nRF52840-DK board and the nRF52840-Dongle.

### Useful commands

Unlocking the nrf device:
```shell
openocd -c 'interface jlink; transport select swd;
source [find target/nrf52.cfg]' -c 'init'  -c 'nrf52_recover'
```

Flash the nRF52840 board:

```shell
BOARD=nrf52840dk make all flash term
```

List all available RIOT modules:

```shell
make info-modules
```


## Project Structure

```shell
project/digitalization
├── Makefile                      # Wrapper Makefile
├── .env                          # Secrets storage file
├── src/
│   ├── Makefile                  # Main Makefile
│   ├── main.c                    # Main application file
│   ├── led_control               # LED control
│   ├── cmd_control               # Shell control
│   ├── further classes           # ...
│   └── utils/
│       ├── Makefile              # Custom module utils
│       ├── timestamp_convert     # Convert timestamps to hh:mm:ss
│       └── error_handler         # Handler error messaging
├── websocket/
│   ├── websocket.py              # Main Websocket file
│   └── further files             # ...
├── CMakeLists.txt                # CMake project file
└── README.md                     # Documentation

```

### Main Class

Entry point of the application that initializes all modules.

Currently, the initialization methods led_control_init(), cpu_temperature_init(), and sensor_mock_init() are NO-OP methods, 
which only return a "success" message. Only cmd_control_init() is actually initializing something: the shell.

### Class cmd_control

Provides the central shell command interface for controlling LEDs, reading CPU temperature, and accessing the mock sensor.

**cmd_control_init**
* Initialize the shell command interface.
* Register the following commands:
  * led \<id> \<action>: Controls LEDs (see [led_control](#class-led_control)).
  * cpu-temp: Reads the CPU temperature (see [cpu_temperature](#class-cpu_temperature)).
  * mock \<temp/hum>: Reads temperature or humidity from the mock sensor (see [sensor_mock](#class-sensor_mock)).


### Class led_control

Manages LED control using SAUL abstraction.

**led_control_init**
* Initializes LED devices.
* NO-OP for SAUL devices, "logs initialization".

**led_saul_write**
* Generates the [phydat_t](#data-type-phydat_t) structure
* Performs the SAUL write operation

**led_control_execute**
* Executes an LED action based on:
  * led_id (0 to 3): The ID of the LED to control.
  * action: One of the following:
    * "on": Turns the LED on.
    * "off": Turns the LED off.
    * Numeric value (e.g., "128"): Sets LED brightness.

### Class cpu_temperature

Reads the CPU temperature data using SAUL abstraction. ([Temp sensor](https://docs.nordicsemi.com/bundle/ps_nrf52840/page/temp.html))

**cpu_temperature_t**
* temperature: The temperature of the CPU.
* scale: Scale of the temperature measurement (10^scale).
* device: Name of the device the measurement is obtained from.
* timestamp: Time of the measurement (Time starts at application start).
* status: Status of the measurement, 0 is success, negative values indicate an [error](#class-error_handler).

**cpu_temperature_init**
* Initializes CPU temperature sensor.
* NO-OP for SAUL devices, "logs initialization".

**cpu_temperature_execute**
* Reads the CPU temperature and returns it as cpu_temperature_t.

**cpu_temperature_print**
* Prints cpu_temperature_t to the console.
* Differentiation between status == 0 and status < 0
* Format for status == 0: [\<time>] The temperature of \<device> is \<temperature> °C
* Format for status < 0: [\<time>] Error: \<error> (Device: \<device>)

### Class sensor_mock

Generates random temperature and humidity values for testing and development purposes.

**sensor_mock_init**
* Initializes mock sensor.
* NO-OP for SAUL devices, "logs initialization".

**generate_mock_data**
* Randomly generates mock data:
  * "temp": Generates a random temperature (0–50°C).
  * "hum": Generates a random humidity (0–100%).

**sensor_mock_execute**
* Reads mock sensor data:
  * "temp": Prints the temperature.
  * "hum": Prints the humidity.

### Utility Classes

These Classes are additional utilities used in the application. They are included into the application as a module.

#### Class error_handler

Defines error codes and provides error messages custom for each error code.

### Class timstamp_convert

Convert a timestamp (&micro;s) into the format hh:mm:ss.


## RIOT-OS Modules
A short description of each module, its purpose, why it was used, and where in the project it is utilized.

### Module SAUL ([S]ensor [A]ctuator [U]ber [L]ayer)

The SAUL module provides a unified abstraction for accessing sensors and actuators. It simplifies interaction with 
devices by exposing a common API for reading and writing values. Ensure compatibility with multiple hardware 
devices using a consistent interface.

* led_control:
  * Uses saul_reg_write to set LED brightness.
  * Uses saul_reg_find_nth to locate the correct LED by its SAUL registry ID.
* cpu_temperature:
  * Uses saul_reg_read to fetch CPU temperature data.

More information here: [SAUL Driver](https://doc.riot-os.org/group__drivers__saul.html) documentation.

### Data Type phydat_t

phydat_t is a structure that standardizes the representation of physical data across sensors and actuators.

| Data Field | Description                         | Example Value | Data Type |
|------------|-------------------------------------|---------------|-----------|
| val[ ]     | Stores (up to) 3-dimensional values | 0.42,0,0      | int16_t   |
| unit       | The (physical) unit of the data     | UNIT_TEMP_C   | uint8_t   |
| scale      | The scale factor (10^factor)        | -2            | int8_t    |                

The example values from the table above result in 0.42 = temp * 10^(-2) UNIT_TEMP_C which means temp = 42°C.

Some sensors provide multidimensional data (e.g. accelerometer) which is why the data field val[ ] is 3-dimensional.

More information here: [phydat_t structure](https://doc.riot-os.org/structphydat__t.html) documentation.

### Module Shell

The shell module provides a command-line interface for interacting with the board. It allows users to issue commands, 
like controlling LEDs or reading sensor values. It provides a simple interface for testing and debugging within 
the project.

* cmd_control:
  * Registers all shell commands (e.g., led, cpu-temp) and dispatches them to their respective handlers.

More information here: [phydat_t structure](https://doc.riot-os.org/group__sys__shell.html) documentation.

<!---
TODO: FUTURE MODULES
--->

### sock.h

A network API for applications and libraries, used to create custom HTTP functionality with the UDP submodule.
See [link](https://doc.riot-os.org/group__net__sock__udp.html).

### jsmn.h
JSON parser library. See [link](https://doc.riot-os.org/group__pkg__jsmn.html).


## Border Router Setup
The IoT device we are using in this project (nRF52840) has BLE (no WLAN or LAN) connectivity only, as these devices 
usually do. Therefore, we have to use a border router which can connect to our device and to a "normal" network. For 
this we are using a raspberry-pi and the nRF52840-Dongle. These two together can be seen as the border router.

The first thing to do is to set up the raspberry-pi and the nRF52840-Dongle. After this is done you can connect the
nRF52840-DK to the nRF52840-Dongle and then establish internet connectivity. 

1. Set up raspberry-pi and nRF52840-Dongle
2. Connect nRF52840-DK board and nRF52840-Dongle
3. ??? Establish internet connectivity

<!---
TODO: INSERT NETWORK DIAGRAM
--->


### Raspberry-Pi Setup


### nRF52840-Dongle Setup

<!---
TODO: REWRITE THIS SECTION

IPv6 lowpan to connect BLE (Bluetooth low energy) of nrf board to standard ipv6, while saving a lot of size for the transmission (e.g. IPv6 header size).
gnrc_networking make all term for interface, use variable PORT (from makefile) to connect if instance already running
Connect to internet:
dist/tools/tapsetup/tapsetup -u \<interface> ; use ethernet as interface to add this to the network

### Raspberry-Pi Setup

ip addr show or /sbin/ifconfig

-> look up address range

sudo nmap -sn 192.168.0.1/24

= gives every device connected to this network

ssh to device -> authenticate with username and password

riot@6lbr-3

(maybe important?):
install kea on raspberry-pi (sudo apt install kea)

https://kea.readthedocs.io/en/latest/arm/dhcp6-srv.html

TODO: HOW TO SETUP RPI.

### nRF52840-Dongle Setup

nrfutil version >=6.1.1 required

-> requires Python >=3.7, <3.11 (https://pypi.org/project/nrfutil/)

Change Makefile in RIOT/examples/gnrc_border_router/Makefile according to
https://teaching.dahahm.de/teaching/ss23/project/2023/05/06/nrf52840dongle_6lbr.html
Use gnrc_border_router from AllRIOT/RIOT repository

set up dongle (from AllRIOT/examples/gnrc_boarder_router) on Linux machine (PC: AMD64 only, no ARM):
BOARD=nrf52840dongle make all flash term

make package (for nrf52840dongle)

find usb device (lsusb), maybe need to push reset button, find target usb port (/dev/ttyACM*, in our case /dev/ttyACM0)

create .zip (for flash): nrfutil pkg generate --hw-version 52 --sd-req 0x00 --application gnrc_border_router.hex --application-version 1 gnrc_border_router.hex.zip
(explain variables!)

flash the device with .zip: nrfutil dfu usb-serial --port /dev/ttyACM0 --package gnrc_border_router.hex.zip

Test with: ...
--->

### Dongle Connectivity

As mentioned above (see [Border Router Setup](#border-router-setup)) the nRF52840-DK board is only directly connected 
to the nRF52840-Dongle. This section explains how this connection can be established.

#### Prerequisites
1. Plug the raspberry-pi into a socket.
2. Connect the raspberry-pi a network via ethernet.
3. Plug the nRF52840-Dongle into one of the USB ports of the raspberry-pi.

#### Raspberry-Pi / nRF52840-Dongle Terminal Setup
1. Connect to the raspberry-pi via ssh and enter the password:
```shell
ssh riot@<network-ip-addr>
```
<!--- MOVE gnrc_border_router TO ANOTHER FOLDER (OUT OF RIOT) AND ADJUST FOLLOWING POINT(S) --->
2. In the raspberry-pi shell (riot@6lbr-3) go to the gnrc_border_router directory:
```shell
cd ~/AllRIOt/examples/gnrc_border_router
```
3. Open the border router terminal on the nRF52840-Dongle (requires a [flashed nRF52840-Dongle](#nrf52840-dongle-setup)):
```shell
BOARD=nrf52840dongle make term
```
4. Get the global ip address of the nRF52840-Dongle; if there are multiple interfaces you can differentiate them by the
other parameters. For example the correct `Link type` is `wireless`. Also make sure to get the IPv6 address with 
`scope: global`.
```shell
> ifconfig
# Iface  6  HWaddr: 6E:0D:84:59:FC:9B 
#           L2-PDU:1500  MTU:1500  HL:64  RTR  
#           Source address length: 6
#           Link type: wired
#           inet6 addr: fe80::6c0d:84ff:fe59:fc9b  scope: link  VAL
#           inet6 addr: aaaa::6c0d:84ff:fe59:fc9b  scope: global  VAL
#           inet6 group: ff02::2
#           inet6 group: ff02::1
#           inet6 group: ff02::1:ff59:fc9b
#           
# Iface  7  HWaddr: 26:3E  Channel: 26  NID: 0x23  PHY: O-QPSK 
#           Long HWaddr: E6:76:0A:9B:E2:59:A6:3E 
#            State: IDLE 
#           ACK_REQ  L2-PDU:102  MTU:1280  HL:64  RTR  
#           RTR_ADV  6LO  IPHC  
#           Source address length: 8
#           Link type: wireless
#           inet6 addr: fe80::e476:a9b:e259:a63e  scope: link  VAL
#           inet6 addr: 2001:470:7347:c318:e476:a9b:e259:a63e  scope: global  VAL
#           inet6 group: ff02::2
#           inet6 group: ff02::1
#           inet6 group: ff02::1:ff59:a63e
#    
```
Correct IPv6 address from this example: `2001:470:7347:c318:e476:a9b:e259:a63e`

#### nRF52840-DK Board Terminal Setup
1. Connect the nRF52840-DK board to your (Linux) PC.
2. Simply start the application as described in [Setup](#setup).
3. Get the global ip address of the nRF52840-DK board; make sure to get the IPv6 address with `scope: global`.
```shell
> ifconfig
# Iface  6  HWaddr: 39:2F  Channel: 26  NID: 0x23  PHY: O-QPSK 
#           Long HWaddr: A6:1D:B3:F5:52:12:39:2F 
#            State: IDLE 
#           ACK_REQ  L2-PDU:102  MTU:1280  HL:64  6LO  
#           IPHC  
#           Source address length: 8
#           Link type: wireless
#           inet6 addr: fe80::a41d:b3f5:5212:392f  scope: link  VAL
#           inet6 addr: 2001:470:7347:c318:a41d:b3f5:5212:392f  scope: global  VAL
#           inet6 group: ff02::1
#           
#           Statistics for Layer 2
#             RX packets 11  bytes 566
#             TX packets 17 (Multicast: 8)  bytes 0
#             TX succeeded 17 errors 0
#           Statistics for IPv6
#             RX packets 10  bytes 696
#             TX packets 17 (Multicast: 8)  bytes 1048
#             TX succeeded 17 errors 0
#
```
Correct IPv6 address from this example: `2001:470:7347:c318:a41d:b3f5:5212:392f`

#### Simple Ping between Dongle and Board
From the border router terminal ([this](#raspberry-pi--nrf52840-dongle-terminal-setup)):
```shell
ping <board-ip-address>
# With the example address from above:
ping 2001:470:7347:c318:a41d:b3f5:5212:392f
```

From the board terminal ([this](#nrf52840-dk-board-terminal-setup)):
```shell
ping <border-router-ip-address>
# With the example address from above:
ping 2001:470:7347:c318:e476:a9b:e259:a63e
```


<!---

connect dongle to raspberry-pi
ssh to rapsberry-pi
go to AllRIOT/examples/gnrc_boarder_router
BOARD=nrf52840dongle make term

connect IoT board (nrf52840dk) to Linux machine (PC)
Use gnrc network example from RIOT for testing (AllRIOT/examples/gnrc_networking)
BOARD=nrf52840dk make all clean flash term
ifconfig
```shell
2024-12-11 13:49:48,151 # Iface  6  HWaddr: 39:2F  Channel: 26  NID: 0x23  PHY: O-QPSK 
2024-12-11 13:49:48,155 #           Long HWaddr: A6:1D:B3:F5:52:12:39:2F 
2024-12-11 13:49:48,157 #            State: IDLE 
2024-12-11 13:49:48,163 #           ACK_REQ  L2-PDU:102  MTU:1280  HL:64  6LO  
2024-12-11 13:49:48,164 #           IPHC  
2024-12-11 13:49:48,167 #           Source address length: 8
2024-12-11 13:49:48,170 #           Link type: wireless
2024-12-11 13:49:48,176 #           inet6 addr: fe80::a41d:b3f5:5212:392f  scope: link  VAL
2024-12-11 13:49:48,182 #           inet6 addr: 2001:db8:0:2:a41d:b3f5:5212:392f  scope: global  VAL
2024-12-11 13:49:48,185 #           inet6 group: ff02::1
2024-12-11 13:49:48,186 #           
2024-12-11 13:49:48,189 #           Statistics for Layer 2
2024-12-11 13:49:48,192 #             RX packets 3  bytes 222
2024-12-11 13:49:48,196 #             TX packets 3 (Multicast: 2)  bytes 0
2024-12-11 13:49:48,199 #             TX succeeded 3 errors 0
2024-12-11 13:49:48,202 #           Statistics for IPv6
2024-12-11 13:49:48,205 #             RX packets 2  bytes 224
2024-12-11 13:49:48,210 #             TX packets 3 (Multicast: 2)  bytes 224
2024-12-11 13:49:48,213 #             TX succeeded 3 errors 0
```
take ip address with "scope: global" from IoT board (nrf52840dk)

on rapsberry-pi console (stil in the "dongle console")
```ping <inet6 addr - scope: global>```
```shell
2024-12-11 13:59:44,241 # 12 bytes from 2001:db8:0:2:a41d:b3f5:5212:392f: icmp_seq=0 ttl=64 rssi=-60 dBm time=9.263 ms
2024-12-11 13:59:45,238 # 12 bytes from 2001:db8:0:2:a41d:b3f5:5212:392f: icmp_seq=1 ttl=64 rssi=-60 dBm time=6.663 ms
2024-12-11 13:59:46,238 # 12 bytes from 2001:db8:0:2:a41d:b3f5:5212:392f: icmp_seq=2 ttl=64 rssi=-60 dBm time=6.025 ms
2024-12-11 13:59:46,238 # 
2024-12-11 13:59:46,243 # --- 2001:db8:0:2:a41d:b3f5:5212:392f PING statistics ---
2024-12-11 13:59:46,247 # 3 packets transmitted, 3 packets received, 0% packet loss
2024-12-11 13:59:46,251 # round-trip min/avg/max = 6.025/7.317/9.263 ms
```

SUCCESS!

--->

### Internet Connectivity

???


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
```shell
led <1-4> <1-4> ... on
```

Turn off an LED:
```shell
led <1-4> <1-4> ... off
```

Toggle an LED:
```shell
led <1-4> <1-4> ... toggle
```

You can control the LEDs by their associated number written on the board (LED1, LED2, LED3, LED4 -> \<1-4>).
Inferentially the maximum number of target LEDs for the commands above is 4.


## TODOs

- [x] Which instant messaging protocol should we use?
    - Telegram
- [ ] Which modules from RIOT-OS do we need?
- [x] Create the project architecture.
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


## Quick Notes

SAUL (Sensor Actuator Uber Layer) API ist wohl wichtig

HTTP from device to (some) Gateway, Gateway translates to HTTPS.
No HTTP on RIOT-OS, simple Telegram HTTP client implementation necessary.

Change Temperature Sensor to onboard (cpu) temperature sensor


## Ask the Prof

### Module ``shell_commands``

[Makefile](src/Makefile) module ``shell_commands`` error:

```shell
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

**Answer**: Yes, use wrapper Makefiles.


# Random Documentation Leftovers

## Temperature Sensor DHT20 (NOT IN USE)
Not supported by RIOT-OS currently, which is why this project is using the CPU temperature.

### PINs
| DHT20 Pin | Function       | nRF52840-DK Pin           |
|-----------|----------------|---------------------------|
| VCC       | Power (3.3V)   | 3V3 on nRF52840-DK        |
| GND       | Ground         | GND                       |
| SDA       | I2C Data Line  | GPIO pin with I2C support |
| SCL       | I2C Clock Line | GPIO pin with I2C support |