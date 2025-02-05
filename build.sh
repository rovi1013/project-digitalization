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

# Function to display help messages
show_help() {
    echo "Script to automate the build and flash process of the application."
    echo "Usage: ./build.sh [option]"
    echo "Options:"
    echo "  --help, -h              Show this help message"
    echo ""
    echo "  default (no option)     Open build and flash menu"
    echo "  --term, -t              Build, flash, and open the RIOT-OS terminal"
    echo "  --native, -n            Build the application for native"
    echo "  --build-only, -b        Build the firmware without flashing"
    echo "  --flash-only, -f        Flash the firmware without building (requires finished build)"
    echo "  --term-only, -T         Only open the RIOT-OS terminal (if the app is running)"
    echo "  --reset-checks, -r      Force system checks again (next run)"
}

# Function to display interactive menu
default_interactive_menu() {
    echo "╔═══════════════════════════════════════════════════════════════╗"
    echo "║  ██████╗  ██╗  ██████╗  ████████╗          ██████╗  ███████╗  ║"
    echo "║  ██╔══██╗ ██║ ██╔═══██╗ ╚══██╔══╝         ██╔═══██╗ ██╔════╝  ║"
    echo "║  ██████╔╝ ██║ ██║   ██║    ██║   ███████╗ ██║   ██║ ███████╗  ║"
    echo "║  ██╔══██╗ ██║ ██║   ██║    ██║   ╚══════╝ ██║   ██║ ╚════██║  ║"
    echo "║  ██║  ██║ ██║  ██████╔╝    ██║             ██████╔╝ ███████║  ║"
    echo "║  ╚═╝  ╚═╝ ╚═╝  ╚═════╝     ╚═╝             ╚═════╝  ╚══════╝  ║"
    echo "║---------------------------------------------------------------║"
    echo "║       📦 PROJECT 'TEXT YOUR IOT DEVICE' BUILD SYSTEM 📦       ║"
    echo "╚═══════════════════════════════════════════════════════════════╝"
    echo ""
    echo "Choose an option:"
    echo "    1) Build and Flash"
    echo "    2) Build Only"
    echo "    3) Flash Only"
    echo "    4) Open Terminal"
    echo "    5) Run Native"
    echo "    6) Exit"
    read -p "Enter your choice: " choice

    case $choice in
        1) system_checks; unlock_nrf_device; build_firmware; flash_firmware; echo "✅ Build process completed!" ;;
        2) system_checks; build_firmware; echo "✅ Building only completed!" ;;
        3) system_checks; unlock_nrf_device; flash_firmware; echo "✅ Flashing only completed!" ;;
        4) open_terminal ;;
        5) system_checks; run_native; ;;
        6) echo "👋 Exiting..."; exit 0 ;;
        *) echo "❌ Invalid choice! Exiting..."; exit 1 ;;
    esac
}

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

    if ip link show "$INTERFACE" &> /dev/null; then
        echo "✅ Interface $INTERFACE is up and running."
    else
        echo "❌ Interface $INTERFACE is DOWN!"
        echo "🔧 Try running: sudo ip tuntap add dev tap0 mode tap user $(whoami) && sudo ip link set tap0 up"
        exit 1
    fi
}

# Function to unlock the nrf device with openocd
unlock_nrf_device() {
    echo "🔓 Unlocking nRF device..."
    openocd -c 'interface jlink; transport select swd; source [find target/nrf52.cfg]' -c 'init'  -c 'nrf52_recover'

    if [ $? -ne 0 ]; then
            echo "❌ Failed to unlock nRF device!"
            exit 1
    fi
}

# Function to check if config.ini exists, and create a template if missing
check_config_file() {
    if [ ! -f "$CONFIG_FILE" ]; then
        echo "❌ Missing config.ini in src/"
        echo "🔧 Please enter your Telegram bot token and chat IDs to create the config.ini file."

        # Prompt for bot token
        while true; do
            read -rp "Enter your Telegram bot token: " BOT_TOKEN
            if [[ -n "$BOT_TOKEN" ]]; then
                break
            fi
            echo "⚠️ Bot token cannot be empty!"
        done

        # Prompt for chat IDs
        while true; do
            read -rp "Enter comma-separated chat IDs (e.g., 12345678,87654321): " CHAT_IDS
            if [[ -n "$CHAT_IDS" ]]; then
                break
            fi
            echo "⚠️ Chat IDs cannot be empty!"
        done

        # Create config.ini with user input
        cat <<EOL > "$CONFIG_FILE"
[telegram]
bot_token = $BOT_TOKEN

[chat_ids]
list = $CHAT_IDS
EOL

        echo "✅ Config file created at: $CONFIG_FILE"
    fi
}

# Function to update chat_ids
update_chat_ids() {
    echo "🚀 Updating chat IDs..."
    python3 "$UPDATE_SCRIPT"
    if [ $? -ne 0 ]; then
        echo "❌ Error updating chat IDs! Exiting..."
        exit 1
    fi
}

# Function build the firmware
build_firmware() {
    echo "🔨 Building firmware..."
    make clean all
    if [ $? -ne 0 ]; then
        echo "❌ Build failed! Exiting..."
        exit 1
    fi
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

# Function to run the application on the BOARD=native
run_native() {
    echo "🔨 Building for native..."
    BOARD=native make clean all term
    if [ $? -ne 0 ]; then
        echo "❌ Build failed! Exiting..."
        exit 1
    fi
}

# Function to reset system checks
reset_checks() {
    rm -f "$CHECKS_DONE_FILE"
    echo "✅ System checks will run again on the next execution."
}

# Run system checks only if this is the first time
system_checks() {
    if [ ! -f "$CHECKS_DONE_FILE" ]; then
        check_packages
        check_interface
        check_config_file
        update_chat_ids
        touch "$CHECKS_DONE_FILE"
    else
        echo "⚡ Skipping system checks (already completed in this session)."
    fi
}

# Default behavior: build and flash
case "$1" in
    --help|-h)
        show_help
        exit 0
        ;;
    --term|-t)
        system_checks
        unlock_nrf_device
        build_firmware
        flash_firmware
        echo "✅ Build process completed!"
        open_terminal
        exit 0
        ;;
    --flash-only|-f)
        system_checks
        unlock_nrf_device
        flash_firmware
        echo "✅ Flashing only completed!"
        exit 0
        ;;
    --build-only|-b)
        system_checks
        build_firmware
        echo "✅ Building only completed!"
        exit 0
        ;;
    --term-only|-T)
        open_terminal
        exit 0
        ;;
    --reset-checks|-r)
        reset_checks
        exit 0
        ;;
    --native|-n)
        system_checks
        run_native
        exit 0
        ;;
    *)
        default_interactive_menu
        exit 0
        ;;
esac