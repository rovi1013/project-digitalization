# Utility Classes Documentation

This is the utility classes documentation and an overview of its classes ([/src/utils](../utils)). These Classes are 
additional utilities used in the application. They are included in the application as a module ([RIOT-OS Documentation](https://doc.riot-os.org/creating-modules.html)).

## Error Handling

The class error_handler defines error codes and provides error messages custom for each error code.
It also handles success messages for the application. The error or success messages include the function 
name the message comes from.

<table>
    <thead>
        <tr>
            <th style="text-align: left;">Type</th>
            <th style="text-align: left;">Class</th>
            <th style="text-align: left;">Name</th>
            <th style="text-align: left;">Description</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td rowspan=4>Success</td>
            <td rowspan="4"></td>
            <td>COAP_SUCCESS</td>
            <td>CoAP message send successful to server</td>
        </tr>
        <tr>
            <td>COAP_PKT_SUCCESS</td>
            <td>CoAP package created successful</td>
        </tr>
        <tr>
            <td>LED_SUCCESS</td>
            <td>LED operation successful</td>
        </tr>
        <tr>
            <td>TEMP_SUCCESS</td>
            <td>Temperature operation successful</td>
        </tr>
        <tr>
            <td rowspan=18>Error</td>
            <td rowspan=4>General</td>
            <td>ERROR_UNKNOWN</td>
            <td>An unknown error occurred</td>
        </tr>
        <tr>
            <td>ERROR_ALLOC_MEMORY_FAIL</td>
            <td>Memory allocation failure detected</td>
        </tr>
        <tr>
            <td>ERROR_NULL_POINTER</td>
            <td>NULL pointer detected in function call</td>
        </tr>
        <tr>
            <td>ERROR_NO_SENSOR</td>
            <td>Sensor not found or unavailable</td>
        </tr>
        <tr>
            <td rowspan=4>Console</td>
            <td>ERROR_INVALID_ARGUMENT</td>
            <td>Invalid argument provided to function</td>
        </tr>
        <tr>
            <td>ERROR_INVALID_ARG_INTERVAL</td>
            <td>Interval must be a positive number</td>
        </tr>
        <tr>
            <td>ERROR_INVALID_ARG_FEEDBACK</td>
            <td>Feedback must be 0 (off) or 1 (on)</td>
        </tr>
        <tr>
            <td>ERROR_INVALID_ARG_PORT</td>
            <td>Port must be a valid number (1-65535)</td>
        </tr>
        <tr>
            <td rowspan=6>Networking</td>
            <td>ERROR_COAP_INIT</td>
            <td>CoAP packet initialization failed</td>
        </tr>
        <tr>
            <td>ERROR_COAP_URI_PATH</td>
            <td>Unable to append URI path in CoAP request</td>
        </tr>
        <tr>
            <td>ERROR_COAP_PAYLOAD</td>
            <td>Payload appending to CoAP request failed</td>
        </tr>
        <tr>
            <td>ERROR_COAP_TIMEOUT</td>
            <td>CoAP request timeout</td>
        </tr>
        <tr>
            <td>ERROR_IPV6_FORMAT</td>
            <td>Invalid IPv6 address format encountered</td>
        </tr>
        <tr>
            <td>ERROR_COAP_SEND</td>
            <td>CoAP request transmission failed</td>
        </tr>
        <tr>
            <td rowspan=1>Configuration</td>
            <td>ERROR_CHAT_ID_NOT_FOUND</td>
            <td>Chat with this ID/person does not exist</td>
        </tr>
        <tr>
            <td rowspan=2>Temperature</td>
            <td>ERROR_TEMP_READ_FAIL</td>
            <td>Temperature data read operation failed</td>
        </tr>
        <tr>
            <td>ERROR_CALLER_UNKNOWN</td>
            <td>Unknown caller function</td>
        </tr>
        <tr>
            <td rowspan=1>LED</td>
            <td>ERROR_LED_WRITE</td>
            <td>Unable to write LED state</td>
        </tr>
    </tbody>
</table>


## Convert Timestamps

The class timestamp_convert converts a timestamp (&micro;s) into the format hh:mm:ss.