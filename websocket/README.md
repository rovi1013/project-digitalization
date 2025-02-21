# Python Websocket Documentation

The Python websocket is running on the raspberry PI which, in combination with the nRF52840-Dongle, is used as a 
border router for this application. Further information on the border router can be found in the [main documentation](../README.md).
But there are no ties between the Raspberry PI as border router and the Raspberry PI as websocket. Technically, you 
could use two different devices for these two applications (border router & websocket). In this documentation, the 
websocket is assumed to be running on the same Raspberry PI as the border router for simplicity reasons.

The Python websocket is a temporary solution to allow communication with Telegram via https. Because currently (Q1 2025) 
RIOT-OS does not support TLS, and therefore is not able to communicate via https. This solution is kept as minimal as 
possible to make a transition to a native solution using RIOT-OS with TLS as easy as possible. 

We have implemented a few "quality of life" features for this temporary solution anyway to make using this 
version as easy to use as possible. 

## Prerequisites 

These prerequisites can be executed manually or via the [setup script](./raspberry_pi_setup.sh) (see [here](#automate-websocket-setup-on-the-raspberry-pi)). 
You can also fulfil some prerequisites manually and use the setup script for the rest. All the setup commands used here 
are meant to be executed from the [/websocket](../websocket) directory. They also ONLY have to be executed on the 
Raspberry PI which is also the border router.

### Python Requirements

Python version 3.12.3 was used throughout this project.

Setup Python environment and install requirements:
```shell
python -m 'venv' .venv
source ./venv/bin/activate
pip install -r requirements.txt
```

### Configuration File

Rename the [configuration file](../src/config.ini.template) to `config.ini`:
```shell
mv ../src/config.ini.template ../src/config.ini
```

And adjust the following setting: 
```ini
[telegram]
bot_token = your_telegram_bot_token
```

### Telegram Password

Create a `.env` file in [/websocket](../websocket):
```shell
touch .env
```

Write the password you want to use for your telegram bot in there:
```dotenv
PASSWORD=<your_password>
```

### Auto-run CoAP Server

The [CoAP Server](./coap_websocket.py) can be set up to run automatically on boot of the Raspberry PI. There are 2 
requirements for this auto-run functionality.

1. Move the [CoAP Server Service](./coap_websocket.service) to `/etc/systemd/system` (requires sudo privileges):
```shell
sudo cp coap_websocket.service /etc/systemd/system
```

2. Append the following lines to `/etc/rc.local` for automatic (sudo) ip address set up on boot:
```shell
sleep 1
ip -6 addr add 2001:470:7347:c810::1234/64 dev usb0
exit 0
```

Make sure there is only one `exit 0` at the very end of the file.

Now you can restart the `daemon` plus enable and restart the _new_ service:
```shell
sudo systemctl daemon-reload
sudo systemctl enable coap_websocket.service
sudo systemctl restart coap_websocket.service
```


## Automate Websocket Setup on the Raspberry PI

To further increase the ease of use and especially ease of setup, you can use the [setup script](./raspberry_pi_setup.sh) 
to automate the setup on the Raspberry PI. This script will automatically run the prerequisites and start the server.

This script requires sudo privileges to run:
```shell
sudo bash raspberry_pi_setup.sh
```

The steps executed by this script are:
1. Copy [coap_websocket.service](./coap_websocket.service) to `/etc/systemd/system/`
2. Modify `/etc/rc.local` to add network settings
3. Reload systemd and enable the service
4. Create Python virtual environment in [.venv](./.venv)
5. Install dependencies from [requirements.txt](./requirements.txt)
6. Rename [config.ini.template](../src/config.ini.template) to [config.ini](../src/config.ini)
7. Set Telegram bot token in [config.ini](../src/config.ini)


## Usage

With the auto-run enabled, there is nothing more to do since the websocket is as minimal as possible by design.

However, you can check the logs of the server either via `journalctl` or by checking the [log file](./coap_server.log) 
which is automatically generated and filled as the server is running.

Using `journalctl` to view life logging:
```shell
journalctl -u coa_websocket.service -f --no-pager
```

<table>
    <thead>
        <tr>
            <th style="text-align: left;">Option</th>
            <th style="text-align: left;">Description</th>
            <th style="text-align: left;">Effect</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>-f</td>
            <td>Follow the journal</td>
            <td>Follow logs in real-time</td>
        </tr>
        <tr>
            <td>--no-pager</td>
            <td>Do not pipe output into a pager</td>
            <td>Print all logs immediately without scrolling or paging</td>
        </tr>
    </tbody>
</table>

For further insight into the communication between the CoAP server and the application you can also check the `tcpdump`:
```shell
sudo tcpdump -i usb0 -nn udp port 5683 -X
```

<table>
    <thead>
        <tr>
            <th style="text-align: left;">Option</th>
            <th style="text-align: left;">Description</th>
            <th style="text-align: left;">Effect</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>-n</td>
            <td>Disables DNS resolution</td>
            <td>Shows IP addresses instead of hostnames</td>
        </tr>
        <tr>
            <td>-nn</td>
            <td>Disables both DNS & port resolution</td>
            <td>Shows raw IPs & port numbers instead of service names</td>
        </tr>
        <tr>
            <td>-X</td>
            <td>Display full packet contents</td>
            <td>Shows packet headers and payload in Hexadecimal and ASCII</td>
        </tr>
    </tbody>
</table>


## Websocket

The [CoAP Websocket](./coap_websocket.py) facilitates communication between the CoAP-based application and the Telegram 
messaging service. It acts as an intermediary, allowing IoT devices using CoAP to send and receive messages through 
Telegram. The server listens for incoming CoAP requests, processes them asynchronously, and communicates with 
Telegram's API.

### Functionalities

**User Registration:**
* New users sending messages get automatically registered (up to 10 users max). 
* Registered users receive updates.

**User Self Remove:**
* Users can send `remove me` via Telegram to unregister.

**Configuration Update:**
* Format: `config <password> <key> <value>`
* Keys: interval, feedback
* Example: `config password12 interval 5`

**Logging:**
* Logs are saved in [coap_server.log](./coap_server.log) with details of requests, errors, and updates.

**Heartbeat:**
* The server logs a heartbeat every 15 minutes to indicate that it is still running.


### API Endpoint POST /message

Handles Telegram message forwarding.

The Payload has to be encoded like this:
```shell
url=<TELEGRAM_API_URL>&token=<BOT_TOKEN>&chat_ids=<CHAT_IDs>&text=<MESSAGE>
```
And chat_ids has to be a comma seperated list of chat_ids: 'chat_id_1,chat_id_2,...,chat_id_10'

**Response Codes**
* 2.05 CONTENT: Messages sent successfully.
* 4.00 BAD REQUEST: Missing required fields.
* 5.00 INTERNAL SERVER ERROR: Processing failure.

### API Endpoint POST /update

Handles Telegram-based configuration retrieval and updates.

The Payload has to be encoded like this:
```shell
url=<TELEGRAM_API_URL>&token=<BOT_TOKEN>&chat_ids=<CHAT_IDs>&text=<MESSAGE>
```
And chat_ids has to be a comma seperated list of chat_ids: 'chat_id_1,chat_id_2,...,chat_id_10'

**Response Codes**
* 2.03 VALID: No updates available.
* 2.05 CONTENT: Updates retrieved successfully.
* 4.00 BAD REQUEST: Missing required fields.
* 5.00 INTERNAL SERVER ERROR: Processing failure.

<!--
Run the application:
```shell
python websocket.py -u"<websocket-url>"
```

Test with python:
```shell
python send_coap_request.py -u"<websocket-url>" -i "<chat-id>" -m "<message>"
```

Get the chat-id from `https://api.telegram.org/bot<actual-telegram_bot-token>/getUpdates`. This will return a list of
all chats with the telegram bot. Search for your username and get the `id` from it. Example website:
```json
{
  "ok": true,
  "result": [
    {
      "update_id": 123456789,
      "message": {
        "message_id": 1,
        "from": {
          "id": 12345678,
          "is_bot": false,
          "first_name": "John",
          "last_name": "Doe",
          "username": "johndoe",
          "language_code": "en"
        },
        "chat": {
          "id": 12345678,
          "first_name": "John",
          "last_name": "Doe",
          "username": "johndoe",
          "type": "private"
        },
        "date": 1678901234,
        "text": "Hello"
      }
    }
  ]
}

```
-->