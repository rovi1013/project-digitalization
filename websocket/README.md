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
TELEGRAM_BOT_TOKEN=<actual_telegram_bot_token>
```


## Usage

Run the application using uvicorn:
```shell
uvicorn websocket:app --reload
```
