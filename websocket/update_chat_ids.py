import requests
import os
import configparser

# Define paths
BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))  # Goes to project directory
CONFIG_FILE = os.path.join(BASE_DIR, "src", "config.ini")  # Path to "src/config.ini"

# Function to load Telegram bot token from config.ini
def load_telegram_bot_token():
    config = configparser.ConfigParser()
    config.read(CONFIG_FILE)

    if "telegram" in config and "bot_token" in config["telegram"]:
        return config["telegram"]["bot_token"]

    print("❌ Error: TELEGRAM_BOT_TOKEN not found in config.ini")
    exit(1)

# Function to fetch chat IDs from Telegram's getUpdates API
def fetch_chat_ids(bot_token):
    url = f"https://api.telegram.org/bot{bot_token}/getUpdates"
    response = requests.get(url)

    if response.status_code != 200:
        print(f"❌ Error fetching updates: {response.text}")
        return []

    updates = response.json().get("result", [])
    chat_ids = set()

    for update in updates:
        if "message" in update:
            chat_id = str(update["message"]["chat"]["id"])
            chat_ids.add(chat_id)

    return list(chat_ids)

# Function to read existing chat IDs from config.ini
def load_existing_chat_ids():
    config = configparser.ConfigParser()
    config.read(CONFIG_FILE)

    if "telegram" in config and "chat_ids" in config["telegram"]:
        return set(config["telegram"]["chat_ids"].split(","))

    return set()

# Function to update config.ini
def update_chat_ids():
    bot_token = load_telegram_bot_token()
    new_chat_ids = fetch_chat_ids(bot_token)

    if not new_chat_ids:
        print("ℹ️ No new chat IDs found.")
        return

    existing_chat_ids = load_existing_chat_ids()
    updated_chat_ids = existing_chat_ids.union(new_chat_ids)

    config = configparser.ConfigParser()
    config.read(CONFIG_FILE)

    if "telegram" not in config:
        config["telegram"] = {}

    config["telegram"]["bot_token"] = bot_token
    config["telegram"]["chat_ids"] = ",".join(sorted(updated_chat_ids))

    with open(CONFIG_FILE, "w") as configfile:
        config.write(configfile)

    print("✅ Updated chat IDs in config.ini")

# Run the script
if __name__ == "__main__":
    update_chat_ids()
