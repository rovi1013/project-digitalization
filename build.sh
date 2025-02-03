#!/bin/bash

# Define paths
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CONFIG_FILE="$SCRIPT_DIR/src/config.ini"
UPDATE_SCRIPT="$SCRIPT_DIR/websocket/update_chat_ids.py"

# Define requirements
REQUIRED_PACKAGES=("git" "gcc-arm-none-eabi" "make" "gcc-multilib" "libstdc++-arm-none-eabi-newlib" "openocd" "gdb-multiarch" "doxygen" "wget" "unzip" "python3-serial")
INTERFACE="tap0"

# Temporary file to track if check have been run already
CHECKS_DONE_FILE="/tmp/build_env_setup_done"

# Function to check for required packages
check_packages() {
    echo "🔍 Checking for required packages..."
    MISSING_PACKAGES=()

    for pkg in "${REQUIRED_PACKAGES[@]}"; do
        if ! dpkg -l | grep -qw "$pkg"; then
            MISSING_PACKAGES+=("$pkg")
        fi
    done

    if [ ${#MISSING_PACKAGES[@]} -ne 0 ]; then
        echo "❌ Missing required packages: ${MISSING_PACKAGES[*]}"
        echo "🔧 Install them with: sudo apt update && sudo apt install ${MISSING_PACKAGES[*]}"
        exit 1
    else
        echo "✅ All required packages are installed."
    fi
}

# Function to check if a tap0 interface is up
check_interface() {
    echo "🔍 Checking if interface $INTERFACE is up..."

    if ip link show "$INTERFACE" | grep -q "state UP"; then
        echo "✅ Interface $INTERFACE is up and running."
    else
        echo "❌ Interface $INTERFACE is DOWN!"
        echo "🔧 Try running: sudo ip tuntap add dev tap0 mode tap user $(whoami) && sudo ip link set tap0 up"
        exit 1
    fi
}

# Function to check if config.ini exists, and create a template if missing
check_config_file() {
    if [ ! -f "$CONFIG_FILE" ]; then
        echo "❌ Error: Missing config.ini in src/"
        echo "🔧 Creating a template config.ini file..."

        # Create config.ini with placeholders
        cat <<EOL > "$CONFIG_FILE"
[telegram]
bot_token = YOUR_BOT_TOKEN_HERE

[chat_ids]
list = 12345678,87654321
EOL

        echo "⚠️ A template has been created at: $CONFIG_FILE"
        echo "🔧 Please open it and enter your Telegram bot token and chat IDs before running the build again."
        exit 1
    fi
}

# Function to update chat_ids and build the firmware
build_firmware() {
    echo "🚀 Updating chat IDs..."
    python3 "$UPDATE_SCRIPT"
    if [ $? -ne 0 ]; then
        echo "❌ Error updating chat IDs! Exiting..."
        exit 1
    fi

    echo "🔨 Building firmware..."
    make clean all
    if [ $? -ne 0 ]; then
        echo "❌ Build failed! Exiting..."
        exit 1
    fi
}

# Function to unlock the nrf device with openocd
unlock_nrf_device() {
    echo "🔍 Unlocking the nrf device..."
    openocd -c 'interface jlink; transport select swd;
    source [find target/nrf52.cfg]' -c 'init'  -c 'nrf52_recover'
}

# Function to flash the firmware
flash_firmware() {
    echo "📡 Flashing firmware..."
    make flash
    if [ $? -ne 0 ]; then
        echo "❌ Flash failed! Exiting..."
        exit 1
    fi
}

# Function to open the RIOT-OS terminal
open_terminal() {
    echo "🖥️ Opening RIOT-OS terminal..."
    make term
}

# Run system checks only if this is the first time
if [ ! -f "$CHECKS_DONE_FILE" ]; then
    check_packages
    check_interface
    check_config_file
    unlock_nrf_device
    touch "$CHECKS_DONE_FILE"
else
    echo "⚡ Skipping system checks (already completed in this session)."
fi

# Default behavior: build and flash
if [ "$1" == "term" ]; then
    build_firmware
    flash_firmware
    echo "✅ Build process completed!"
    open_terminal
elif [ "$1" == "flash-only" ]; then
    flash_firmware
    echo "✅ Flashing completed!"
elif [ "$1" == "build-only" ]; then
    build_firmware
    echo "✅ Building completed!"
elif [ "$1" == "term-only" ]; then
    open_terminal
else
    build_firmware
    flash_firmware
    echo "✅ Build process completed!"
fi
