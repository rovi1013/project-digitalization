# Main Classes Documentation

This is the main application's documentation and an overview of its classes ([/src](../src)).

## Main Class

Entry point of the application that initializes all modules.

Currently, the initialization methods led_control_init(), cpu_temperature_init() are NO-OP methods,
which only return a "success" message. Only cmd_control_init() is actually initializing something: the shell.

## Class cmd_control

Provides the central shell command interface for controlling LEDs, reading CPU temperature.

### cmd_control_init
* Initialize the shell command interface.
* Register the following commands:
    * led \<id> \<action>: Controls LEDs (see [led_control](#class-led_control)).
    * cpu-temp: Reads the CPU temperature (see [cpu_temperature](#class-cpu_temperature)).
    * coap-test: Sends a test message to the telegram bot.


## Class led_control

Manages LED control using SAUL abstraction.

### led_control_init
* Initializes LED devices.
* NO-OP for SAUL devices, "logs initialization".

### led_saul_write
* Generates the phydat_t structure (see **RIOT-OS Modules** in [README](../README.md))
* Performs the SAUL write operation

### led_control_execute
* Executes an LED action based on:
    * led_id (0 to 3): The ID of the LED to control.
    * action: One of the following:
        * "on": Turns the LED on.
        * "off": Turns the LED off.
        * Numeric value (e.g., "128"): Sets LED brightness.

## Class cpu_temperature

Reads the CPU temperature data using SAUL abstraction. ([Temp sensor](https://docs.nordicsemi.com/bundle/ps_nrf52840/page/temp.html))

### cpu_temperature_t
* temperature: The temperature of the CPU.
* scale: Scale of the temperature measurement (10^scale).
* device: Name of the device the measurement is obtained from.
* timestamp: Time of the measurement (Time starts at application start).
* status: Status of the measurement, 0 is success, negative values indicate an error (see **Error Handling** in [Utility README](./utils/README.md)).

### cpu_temperature_init
* Initializes CPU temperature sensor.
* NO-OP for SAUL devices, "logs initialization".

### cpu_temperature_execute
* Reads the CPU temperature and returns it as cpu_temperature_t.

### cpu_temperature_print
* Prints cpu_temperature_t to the console.
* Differentiation between status == 0 and status < 0
* Format for status == 0: [\<time>] The temperature of \<device> is \<temperature> °C
* Format for status < 0: [\<time>] Error: \<error> (Device: \<device>)

## Class coap_control

Sends CoAP-requests and handles the responses.

### coap_control
* The main method that needs to be called for sending requests
* Expects 6 arguments:
  * command: The command used to access the control
  * type: The request type (GET|POST) supported
  * address: The ipv6-adress of the websocket
  * port: The port of the websocket
  * path: The path of the websocket (e.g. /send_message)
  * data: The target destination information (the Telegram Server) required by the websocket as well as the chat_ids and the text message
* Initializes the request by utilizing the gcoap library
* Prepares the payload in case of a POST request
* Sends the message via _send and checks if the sending was successfull

### _send
* Sends the request to the given destination
* Expects 6 arguments:
  * buf: Buffer
  * len: Length
  * *addr_str: Adress
  * *port_str: Port
  * *ctx: Context
  * tl: Socket Type
* Utilizes _parse_endpoint to reformat the given adress and port for gcoap. The details are saved in the variable remote
* The number of bytes sent is returned

### _sending
* An alternative implementation of _send 
* Necessary for the response handling as the server adress and port are not available as a string
* TODO: Needs to be merged with _send by reworking the code base of coap_control

### _parse_endpoint
* Parses a given server adress and port into a readable format for gcoap
* Utilzes the netutils package to save the new details in the variable remote

### _resp_handler
* Handler to take care of any response received from the websocket
* Necessary to investigate potential issues like timeouts or error codes