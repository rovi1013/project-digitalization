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

## Class coap_post

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


# Class configuration



## Header config_constants

This header is used for 3 different functionalities regarding important constants of the application.
* Define constants which limit the size of certain variables used throughout the application. 
* Set the default values for variables in case they were not assigned at build-time as CFLAGS.
* Throw an error at build-time if the 2 most important variables are missing

<table>
    <thead>
        <tr>
            <th style="text-align: left;">Type</th>
            <th style="text-align: left;">Name</th>
            <th style="text-align: left;">Value</th>
            <th style="text-align: left;">Description</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td rowspan=9>Constant Lengths</td>
            <td>BOT_TOKEN_LENGTH</td>
            <td>50</td>
            <td>The length of the telegram bot token.</td>
        </tr>
        <tr>
            <td>MAX_CHAT_IDS</td>
            <td>10</td>
            <td>The maximum number of telegram chats.</td>
        </tr>
        <tr>
            <td>CHAT_ID_LENGTH</td>
            <td>12</td>
            <td>The length of a single telegram chat id.</td>
        </tr>
        <tr>
            <td>CHAT_NAME_LENGTH</td>
            <td>15</td>
            <td>The length of the associated first name to the chat id.</td>
        </tr>
        <tr>
            <td>URL_LENGTH</td>
            <td>30</td>
            <td>The length of the telegram bot url.</td>
        </tr>
        <tr>
            <td>ADDRESS_LENGTH</td>
            <td>40</td>
            <td>The length of the IPv6 address from the CoAP server.</td>
        </tr>
        <tr>
            <td>PORT_LENGTH</td>
            <td>5</td>
            <td>The length of the CoAP server port.</td>
        </tr>
        <tr>
            <td>URI_PATH_LENGTH</td>
            <td>20</td>
            <td>The length of the CoAP server endpoint.</td>
        </tr>
        <tr>
            <td>MESSAGE_DATA_LENGTH</td>
            <td>40</td>
            <td>The maximum size of the actual message payload.</td>
        </tr>
        <tr>
            <td rowspan=6>Default Values</td>
            <td>TEMPERATURE_NOTIFICATION_INTERVAL</td>
            <td>5</td>
            <td>Temperature notification interval (in min) used to send telegram messages to the user.</td>
        </tr>
        <tr>
            <td>ENABLE_LED_FEEDBACK</td>
            <td>0</td>
            <td>Toggle to en-/disable LED feedback on CoAP messages.</td>
        </tr>
        <tr>
            <td>TELEGRAM_SERVER_URL</td>
            <td>"https://api.telegram.org/bot"</td>
            <td>The telegram bot URL.</td>
        </tr>
        <tr>
            <td>COAP_SERVER_ADDRESS</td>
            <td>"::1"</td>
            <td>The IPv6 address of the CoAP server.</td>
        </tr>
        <tr>
            <td>COAP_SERVER_PORT</td>
            <td>"5683"</td>
            <td>The port of the CoAP server.</td>
        </tr>
        <tr>
            <td>COAP_SERVER_URI_PATH</td>
            <td>"/message"</td>
            <td>The endpoint of the CoAP server.</td>
        </tr>
        <tr>
            <td rowspan=2>Important Variables</td>
            <td>TELEGRAM_BOT_TOKEN</td>
            <td>-</td>
            <td>The telegram bot token.</td>
        </tr>
        <tr>
            <td>TELEGRAM_CHAT_IDS</td>
            <td>-</td>
            <td>The telegram username and chat_id combination.</td>
        </tr>
    </tbody>
</table>