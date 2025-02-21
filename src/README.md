# Main Classes Documentation

This is the main application's documentation and an overview of its classes ([/src](../src)).

## Main Class

Entry point of the application. The `main()` function is created as a separate thread which is always running. 
Here we initialize the Console and the CoAP threads. These two threads are defined in the main class too. As a safety 
precaution, the main thread will wait 1 additional second after everything is started.

### Console Thread

The Console thread is only running if the CFLAG `ENABLE_CONSOLE_THREAD` is set to 1. This thread initializes the console 
class, which in turn enables the usage of the RIOT-OS shell. The reason this thread can be disabled is that the shell 
doesn't need to be accessed during "standard" operation, where the application is simply sending temperature messages 
periodically. The console is always running because the RIOT-OS function `shell_run_forever()` 
([documentation](https://doc.riot-os.org/shell_8h.html)) runs a `while(1)` loop and this is the RIOT-OS function that 
manages the RIOT-OS shell itself. To keep the battery consumption as low as possible, this thread should be disabled 
during "standard" operation.

### CoAP Thread

The CoAP thread is always started. Runs in a continuous loop (`while(1)`), but the thread sleeps for most of the time. 
The thread executes the following steps:

1: First coap_get new config

2: Measure current temp

3: Then coap_post sending notification

4: Sleep for `app_config.temperature_notification_interval` minutes  

Repeat steps 1-4

Additional Feature: LED Feedback (Toggle via `app_config.enable_led_feedback`)


## Class cmd_control

Provides the central shell command interface for controlling LEDs, reading CPU temperature.

### cmd_control_init

* Initialize the shell command interface.
* Register the following commands:
    * led \<id> \<action>: Controls LEDs.
    * cpu-temp: Reads the CPU temperature.
    * coap-send \<recipient> \<message>: Send a message to the telegram bot.
    * config \<name> \<operation>: change a configuration variable.

### led_control

Set the value = \<action> of the LED with ID = \<id>. Where an action can be one of the following values:
* on: Set the value of the LED to 255.
* off: Set the value of the LED to 0.
* range 0-255: Set the value to an Integer between 0 and 255.
  
For more details, see [led_control](#class-led_control).


### cpu_temp_control

Read the temperature of a SAUL device of the type `SAUL_SENSE_TEMP`. For `BOARD=native` read a mock sensor. For more 
details, see [cpu_temperature](#class-cpu_temperature).

### coap_send_control

Send the \<message> to the specified telegram user in \<recipient>. Set \<recipient> to "all" to send the message to all
(currently registered) telegram users. If configured, this will also trigger the LED Feedback while the CoAP message is 
being processed.

For more details, see [coap_post](#class-coap_post).

### modify_config

This function allows the user to change the configuration settings during runtime if the console thread is enabled. 
And you can print the current configuration settings using `config show`. Additionally, `config help` will print out 
all the possible commands using `config ...`.

<table>
    <thead>
        <tr>
            <th style="text-align: left;">Command Name</th>
            <th style="text-align: left;"  colspan=2>Parameters</th>
            <th style="text-align: left;">Description</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>help</td>
            <td colspan=2>-</td>
            <td>Show the usage of the config command.</td>
        </tr>
        <tr>
            <td>show</td>
            <td colspan=2>-</td>
            <td>Show the current configuration settings.</td>
        </tr>
        <tr>
            <td>interval</td>
            <td colspan=2>minutes</td>
            <td>Set temperature notification interval.</td>
        </tr>
        <tr>
            <td>feedback</td>
            <td colspan=2>1 or 0</td>
            <td>Enable/disable LED feedback.</td>
        </tr>
        <tr>
            <td>bot-token</td>
            <td colspan=2>token</td>
            <td>Set Telegram bot token.</td>
        </tr>
        <tr>
            <td>set-chat</td>
            <td>name</td>
            <td>id</td>
            <td>Create a new name-ID pair or update an existing one.</td>
        </tr>
        <tr>
            <td>remove-chat</td>
            <td colspan=2>ID or name</td>
            <td>Remove a chat entry by ID or name.</td>
        </tr>
        <tr>
            <td>telegram-url</td>
            <td colspan=2>url</td>
            <td>Set Telegram bot API URL.</td>
        </tr>
        <tr>
            <td>address</td>
            <td colspan=2>IPv6</td>
            <td>Set CoAP server address.</td>
        </tr>
        <tr>
            <td>port</td>
            <td colspan=2>port</td>
            <td>Set CoAP server port.</td>
        </tr>
        <tr>
            <td>uri-path</td>
            <td colspan=2>path</td>
            <td>Set CoAP server URI path.</td>
        </tr>
    </tbody>
</table>

For more details, see [configuration](#class-configuration).


## Class led_control

Manages LED control using SAUL abstraction. Normally, you can access devices using SAULs preconfigured enumerators, like
SAUL_SENSE_TEMP for a temperature sensor. This allows dynamically accessing any sensor by its type instead of its 
board-specific ID. SAUL performs the translation from sensor type to the matching ID. In the case of (non RGB) LEDs the 
SAUL abstraction layer has no such definitions. Instead, it only registers LEDs by their IDs. This impedes the 
ability to dynamically access LED sensors on IoT boards using SAUL. Hence, the LED control will only work on boards, 
with the same LED IDs as the nRF52840-DK board. These IDs are #4 - #7.

SAUL devices on the nRF52840-DK board, command `saul read all`:
```shell
# Reading from #0 (Button 1|SENSE_BTN)
# Data:	              0 
# 
# Reading from #1 (Button 2|SENSE_BTN)
# Data:	              0 
# 
# Reading from #2 (Button 3|SENSE_BTN)
# Data:	              0 
# 
# Reading from #3 (Button 4|SENSE_BTN)
# Data:	              0 
# 
# error: failed to read from device #4
# 
# error: failed to read from device #5
# 
# error: failed to read from device #6
# 
# error: failed to read from device #7
# 
# Reading from #8 (NRF_TEMP|SENSE_TEMP)
# Data:	          23.75 °C
```
Here you can see the SAUL types in the brackets behind each ID. And for the LEDs, IDs #4 - #7, there are no such types 
listed here. Also, the RIOT-OS documentation regarding SAUL has no (non RGB) LEDs listed [here](https://doc.riot-os.org/group__drivers__saul.html).

### led_saul_write
* Generates the phydat_t structure (see **RIOT-OS Modules** in [README](../README.md))
* Performs the SAUL write operation

For `BOARD=native` we simply print a message, pretending to set the value of the specified LED ID.

### led_control_execute
* Execute an LED action based on:
    * led_id (0 to 3): The ID of the LED to control.
    * action: One of the following:
        * "on": Turns the LED on.
        * "off": Turns the LED off.
        * Integer value 0-255 (e.g., "128"): Sets LED brightness.


## Class cpu_temperature

Read the CPU temperature data using SAUL abstraction. This allows accessing any sensor using its type instead of a 
board-specific ID, in the case of a temperature sensor the SAUL type is called `SAUL_SENSE_TEMP`. This allows for a 
flexible management independent of the specific board used. 

In the context of this project, using the nRF52840-DK, the temperature sensor has the ID #8. The specific temperature 
sensor of nRF52840 boards is a die temperature sensor (DTS), meaning it measures the Chip/CPU temperature 
([Temp sensor](https://docs.nordicsemi.com/bundle/ps_nrf52840/page/temp.html)).

### cpu_temperature_t

A struct to store additional information compared to the default phydat_t struct (see **RIOT-OS Modules** in [README](../README.md)) 
used by SAUL.

<table>
    <thead>
        <tr>
            <th style="text-align: left;">Name</th>
            <th style="text-align: left;">Type</th>
            <th style="text-align: left;">Description</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>temperature</td>
            <td>int16_t</td>
            <td>The temperature of the device.</td>
        </tr>
        <tr>
            <td>scale</td>
            <td>int8_t</td>
            <td>Scale of the temperature measurement (10^scale).</td>
        </tr>
        <tr>
            <td>device</td>
            <td>char</td>
            <td>Name of the device the measurement is obtained from.</td>
        </tr>
        <tr>
            <td>timestamp</td>
            <td>char</td>
            <td>Time of the measurement (Time starts at application start).</td>
        </tr>
        <tr>
            <td>status</td>
            <td>char</td>
            <td>Status of the measurement using custom error codes.</td>
        </tr>
    </tbody>
</table>

More information about errors: see **Error Handling** in [Utility README](./utils/README.md).

### caller_class_t

Define different caller classes for the [cpu_temperature_formatter](#cpu_temperature_formatter) string formatting.

### unit_map_t

Provide a struct to be able to assign each unit a corresponding string.

### unit_to_string

Use the `temperature_unit_map` of type `unit_map_t` to return a pointer to a corresponding unit string.

### cpu_temperature_get
* Is called using a pointer to a cpu_temperature_t struct.
* Read temperature value of the `SAUL_SENSE_TEMP` sensor.
* Write information to the provided cpu_temperature_t struct.

In case of `BOARD=native` the provided cpu_temperature_t struct is filled with mock values.

### determine_divisor

This is a utility function that allows to format the temperature data. The temperature data is provided as an Integer
value (e.g. 2500) and a scale factor (e.g. -2). These two values have to be combined the form a readable temperature. 
This function provides the divisor which is like this: temperature_value / divisor = readable_temperature. The divisor 
is assigned using the scale factor. The advantage of this method is that we can assign the resulting value 
(readable_temperature) to a string without the need of float values, but by simply calculating the integer part and the 
fractional part separately. 

The calculation using these values looks like this: 
* temperature_value = 2500, scale_factor = -2 -> divisor = 100  
* 2500 * 10^(-2) = 2500 * 0.01 = 2500 / 100. 

This function only provides the matching divisor to a scale factor, the calculations are all done in
[cpu_temperature_formatter](#cpu_temperature_formatter).

### cpu_temperature_formatter

* Is called with the following parameters:
  * A pointer to a cpu_temperature_t struct
  * The caller class of this function
  * A buffer for the formatted string
  * The size of this buffer

Calculate the divisor, integer and fractional part of the temperature reading as described [here](#determine_divisor). 
The caller class determines the format of the output. Keeping the CoAP message payload as small as possible without 
sacrificing information while the message formated for the shell output contains additional debugging information. 

The formatting of the temperature itself looks like this: `%d.%0*d`. There are the 2 integer parts with the first one 
being the integer from the temperature and the second one the fractional part. Depending on the scale variable, the 
minimum number of decimal places is determined. The minimum number of decimal places is the inverse of the scale value
(scale = -2 -> min decimal places = 2).


## Class coap_post

Sends CoAP-requests and handles the responses.

### set_coap_response_status
* Setter for the variable coap_response_status

### get_coap_response_status
* Getter for the variable coap_response_status

### process_config_command
* Analyzes a given token and configures variables from config.ini accordingly
* Expects 1 argument:
  * *token: The token that needs to be analyzed
* The configuration is performed by calling the corresponding functions from configuration.h

### config_control
* Prepares the payload data and divides it into substrings if necessary
* Expects 1 argument:
  * *pkt: The CoAP packet
* Copies the payload of the websocket response into the variable named "response"
* Checks if the response contains an expected message that needs to further analysis
* If the message contains configuration info, it is divided into substrings, if necessary, and passed on to process_config_command

### coap_response_handler
* Handles the incoming responses from the websocket
* Expects 3 arguments:
  * *memo: The request memo type
  * *pkt: The CoAP packet
  * *remote: Target Destination (the Websocket)
* Gets context from memo
* Handles Timeouts
* Handles Acknowledgements
* Handles Payloads
* Handles Block wise response handling

### coap_prepare_packet
* Prepares the packet before it is being sent
* Expects 3 arguments
  * *pkt: The CoAP packet
  * *uri_path: The path to the websocket
  * *payload: The payload for the transmission
* The request is created with gcoap
* The message type is set to Confirmable
* Then the payload is added to the request

### coap_send_request
* Sends the request to target destination
* Expects 1 argument
  * *pkt: The CoAp packet
* Preparing the CoAP destination
* Sending the Request to the target destination
* On error an error message is returned, otherwise a success statement

### coap_post_send
* The control function that orcestrates all sub processes
* Expects 2 arguments
  * *message: The message to send
  * *recipient: The chat_ids (the recipients) of the message
* First the URI Path is built
* Then the chat ID(s) are determined
* After that the payload is built
* Followed by the preparation of the CoAP packet
* Finally, the request is sent

### coap_post_get_updates
* Similar to coap_post_send, but it does not require input
* Is used to fetch updates from the bot by calling getUpdates via the websocket


## Class configuration

This class functions as the central configuration management. The variable app_config uses the struct config_t to store 
the configuration variables. 

The struct config_t:

<table>
    <thead>
        <tr>
            <th style="text-align: left;">Name</th>
            <th style="text-align: left;">Type</th>
            <th style="text-align: left;">Description</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>temperature_notification_interval</td>
            <td>int</td>
            <td>Temperature notification interval in minutes.</td>
        </tr>
        <tr>
            <td>enable_led_feedback</td>
            <td>bool</td>
            <td>Led Feedback toggle.</td>
        </tr>
        <tr>
            <td>bot_token</td>
            <td>char</td>
            <td>Telegram bot token.</td>
        </tr>
        <tr>
            <td>chat_ids</td>
            <td>chat_entry_t</td>
            <td>Telegram chat usernames and ids.</td>
        </tr>
        <tr>
            <td>telegram_url</td>
            <td>char</td>
            <td>Telegram API URL.</td>
        </tr>
        <tr>
            <td>address</td>
            <td>char</td>
            <td>CoAP server IPv6 address.</td>
        </tr>
        <tr>
            <td>port</td>
            <td>char</td>
            <td>CoAP server port.</td>
        </tr>
        <tr>
            <td>uri_path</td>
            <td>char</td>
            <td>CoAP server URI path.</td>
        </tr>
    </tbody>
</table>

The struct chat_entry_t:

<table>
    <thead>
        <tr>
            <th style="text-align: left;">Name</th>
            <th style="text-align: left;">Type</th>
            <th style="text-align: left;">Description</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>first_name</td>
            <td>char</td>
            <td>First name of the person associated with a chat.</td>
        </tr>
        <tr>
            <td>chat_id</td>
            <td>char</td>
            <td>Chat id of a chat</td>
        </tr>
    </tbody>
</table>

This class further provides setter and getter functions for all of these variables. To allow for modification and 
management of these variables during runtime.

In the case of chat_ids there is some additional functionality implemented. For each of the following functionalities 
is implemented in a separate function:
* You can add or update chat_ids by first_name and by chat_id
* You can get a single chat_id by first_name
* You can get a single chat_id by index
* You can get a list of all chat_ids (comma seperated)
* You can remove a chat_id from chat_ids by chat_id or first_name


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