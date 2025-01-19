# Python Websocket Documentation

## Prerequisites 

### Python Requirements

Python TODO:VERSION

Setup Python environment (linux):
```shell
python -m 'venv' .venv
source ./venv/bin/activate
pip install -r requirements.txt
```

### Environment Variables

Create a file called `.env` in [/websocket](../websocket):
```shell
touch .env
```

And copy the telegram bot token into it like this:
```dotenv
TELEGRAM_BOT_TOKEN=<actual-telegram_bot-token>
```


## Usage

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
