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
        return {}

    updates = response.json().get("result", [])
    chat_id_map = {}

    for update in updates:
        if "message" in update:
            chat_id = str(update["message"]["chat"]["id"])
            first_name = update["message"]["chat"].get("first_name", f"User{chat_id}")  # Default to "User<ID>"
            chat_id_map[chat_id] = first_name  # Store chat ID with name

    return chat_id_map

# Function to read existing chat IDs from config.ini
def load_existing_chat_ids():
    config = configparser.ConfigParser()
    config.read(CONFIG_FILE)

    if "telegram" in config and "chat_ids" in config["telegram"]:
        chat_id_entries = config["telegram"]["chat_ids"].split(",")
        chat_id_map = {}

        for entry in chat_id_entries:
            if ":" in entry:
                name, chat_id = entry.split(":", 1)
                chat_id_map[chat_id] = name  # Store name with chat ID

        return chat_id_map

    return {}

# Function to update config.ini
def update_chat_ids():
    bot_token = load_telegram_bot_token()
    new_chat_id_map = fetch_chat_ids(bot_token)

    if not new_chat_id_map:
        print("ℹ️  No new chat IDs found.")
        return

    existing_chat_id_map = load_existing_chat_ids()

    # Merge existing names with new IDs
    for chat_id, name in new_chat_id_map.items():
        if chat_id not in existing_chat_id_map:
            existing_chat_id_map[chat_id] = name  # Add new entry

    # Format chat IDs as "Name:ID"
    formatted_chat_ids = [f"{name}:{chat_id}" for chat_id, name in existing_chat_id_map.items()]

    # Update config.ini
    config = configparser.ConfigParser()
    config.read(CONFIG_FILE)

    if "telegram" not in config:
        config["telegram"] = {}

    config["telegram"]["bot_token"] = bot_token
    config["telegram"]["chat_ids"] = ",".join(sorted(formatted_chat_ids))  # Sort for consistency

    with open(CONFIG_FILE, "w", encoding="utf-8") as configfile:
        config.write(configfile)

    print("✅ Updated chat IDs in config.ini")

# Run the script
if __name__ == "__main__":
    update_chat_ids()
