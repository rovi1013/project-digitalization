#!/bin/bash

# Ensure the script is run with sudo
if [ "$(id -u)" -ne 0 ]; then
    echo "❌ This script must be run as root. Please use sudo!"
    exit 1
fi

# Prompt user for confirmation
clear
echo "╔═══════════════════════════════════════════════════════════════╗"
echo "║  ██████╗  ██╗  ██████╗  ████████╗          ██████╗  ███████╗  ║"
echo "║  ██╔══██╗ ██║ ██╔═══██╗ ╚══██╔══╝         ██╔═══██╗ ██╔════╝  ║"
echo "║  ██████╔╝ ██║ ██║   ██║    ██║   ███████╗ ██║   ██║ ███████╗  ║"
echo "║  ██╔══██╗ ██║ ██║   ██║    ██║   ╚══════╝ ██║   ██║ ╚════██║  ║"
echo "║  ██║  ██║ ██║  ██████╔╝    ██║             ██████╔╝ ███████║  ║"
echo "║  ╚═╝  ╚═╝ ╚═╝  ╚═════╝     ╚═╝             ╚═════╝  ╚══════╝  ║"
echo "║---------------------------------------------------------------║"
echo "║    🍓 PROJECT 'TEXT YOUR IOT DEVICE' RASPBERRY PI SETUP 🍓    ║"
echo "╚═══════════════════════════════════════════════════════════════╝"

echo ""
echo "╔═══════════════════════════════════════════════════════════════╗"
echo "║ ⚡ This script will perform the following actions:            ║"
echo "║---------------------------------------------------------------║"
printf "║ %-69s ║\n" "1️⃣  Copy    📄 coap_websocket.service to /etc/systemd/system/"
printf "║ %-72s ║\n" "2️⃣  Modify  ✏️  /etc/rc.local to add network settings"
printf "║ %-69s ║\n" "3️⃣  Reload  🔄 systemd and enable the service"
printf "║ %-69s ║\n" "4️⃣  Create  🐍 Python virtual environment in .venv"
printf "║ %-69s ║\n" "5️⃣  Install 📦 dependencies from requirements.txt"
printf "║ %-69s ║\n" "6️⃣  Rename  🔄 config.ini.template to config.ini"
printf "║ %-69s ║\n" "7️⃣  Set     🔑 Telegram bot token in config.ini"
echo "╚═══════════════════════════════════════════════════════════════╝"

echo ""
read -p "❓ Are you sure you want to proceed? (y/N): " CONFIRM

# Convert input to lowercase and check if it's 'y' or 'yes'
CONFIRM=$(echo "$CONFIRM" | tr '[:upper:]' '[:lower:]')
if [[ "$CONFIRM" != "y" && "$CONFIRM" != "yes" ]]; then
    echo "🚫 Operation canceled by the user."
    exit 0
fi

# Define paths
SERVICE_FILE="coap_websocket.service"
SYSTEMD_PATH="/etc/systemd/system/"
RC_LOCAL="/etc/rc.local"
VENV_DIR=".venv"
REQUIREMENTS_FILE="requirements.txt"
CONFIG_TEMPLATE="../src/config.ini.template"
CONFIG_FILE="../src/config.ini"

# Copy the service file
echo "📂 Copying $SERVICE_FILE to $SYSTEMD_PATH"
cp "$SERVICE_FILE" "$SYSTEMD_PATH"

# Ensure rc.local exists
if [ ! -f "$RC_LOCAL" ]; then
    echo "⚠️  /etc/rc.local not found. Creating a new one..."
    echo "#!/bin/bash" > "$RC_LOCAL"
    echo "exit 0" >> "$RC_LOCAL"
fi

# Append lines to rc.local if not already present
if ! grep -q "sleep 1" "$RC_LOCAL"; then
    sed -i '/^exit 0/i sleep 1' "$RC_LOCAL"
fi

if ! grep -q "ip -6 addr add 2001:470:7347:c810::1234/64 dev usb0" "$RC_LOCAL"; then
    echo "🌐 Adding IPv6 configuration to $RC_LOCAL"
    sed -i '/^exit 0/i ip -6 addr add 2001:470:7347:c810::1234/64 dev usb0' "$RC_LOCAL"
fi

# Reload systemd and enable the service
echo "🔄 Reloading systemd daemon and enabling service"
systemctl daemon-reload
systemctl enable coap_websocket.service
systemctl restart coap_websocket.service

# Install Python and create virtual environment
echo "🐍 Checking if Python and virtualenv are installed..."
if ! command -v python3 &>/dev/null; then
    echo "❌ Python3 is not installed. Installing..."
    sudo apt update && sudo apt install -y python3 python3-venv python3-pip
else
    echo "✅ Python3 is installed."
fi

# Create the virtual environment if it doesn't exist
if [ ! -d "$VENV_DIR" ]; then
    echo "📦 Creating Python virtual environment in $VENV_DIR..."
    python3 -m venv "$VENV_DIR"
else
    echo "⚠️ Virtual environment already exists in $VENV_DIR. Skipping creation."
fi

# Activate virtual environment and install requirements
if [ -f "$REQUIREMENTS_FILE" ]; then
    echo "📜 Installing dependencies from $REQUIREMENTS_FILE..."
    "$VENV_DIR/bin/python" -m pip install --upgrade pip
    "$VENV_DIR/bin/python" -m pip install -r "$REQUIREMENTS_FILE"
    echo "✅ Dependencies installed successfully."
else
    echo "⚠️ No requirements.txt found. Skipping dependency installation."
fi

# Rename config.ini.template to config.ini if it hasn't been renamed yet
if [ -f "$CONFIG_TEMPLATE" ]; then
    if [ ! -f "$CONFIG_FILE" ]; then
        echo "📄 Renaming $CONFIG_TEMPLATE to $CONFIG_FILE..."
        mv "$CONFIG_TEMPLATE" "$CONFIG_FILE"
    else
        echo "⚠️ $CONFIG_FILE already exists. Skipping rename."
    fi
else
    echo "❌ Configuration template not found! Skipping this step."
fi

# Prompt user for the Telegram bot token
if [ -f "$CONFIG_FILE" ]; then
    echo "✏️  Please enter your Telegram bot token:"
    read -r TELEGRAM_BOT_TOKEN

    # Ensure user entered something
    if [ -n "$TELEGRAM_BOT_TOKEN" ]; then
        # Replace the placeholder in config.ini
        sed -i "s|bot_token = your_telegram_bot_token|bot_token = $TELEGRAM_BOT_TOKEN|" "$CONFIG_FILE"
        echo "✅ Telegram bot token has been set in $CONFIG_FILE."
    else
        echo "⚠️ No bot token entered. Skipping update."
    fi
fi

echo "✅ Setup completed successfully! 🚀"
