#!/bin/bash

# Define paths
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CONFIG_FILE="$SCRIPT_DIR/src/config.ini"
UPDATE_SCRIPT="$SCRIPT_DIR/websocket/update_chat_ids.py"
CHECKS_DONE_FILE="/tmp/build_env_setup_done"

# Define requirements
REQUIRED_PACKAGES=("git" "gcc-arm-none-eabi" "make" "gcc-multilib" "libstdc++-arm-none-eabi-newlib" "openocd" "gdb-multiarch" "doxygen" "wget" "unzip" "python3-serial")
INTERFACE="tap0"

# Default Environment Variables
BOARD="nrf52840dk"
ENABLE_CONSOLE_THREAD=0
VERBOSE=0
LOG_FILE="build_script.log"

################################################################################
##################################### HELP #####################################
################################################################################

# Function to display the help page
show_help() {
    clear
    echo "╔═══════════════════════════════════════════════════════════════╗"
    echo "║                         ℹ️  HELP PAGE                          ║"
    echo "╚═══════════════════════════════════════════════════════════════╝"

    echo "This script automates building, flashing, and interacting with"
    echo "the IoT application. Below are the available options:"

    echo "╔═══════════════════════════════════════════════════════════════╗"
    printf "║ %-63s ║\n" "1) 🚀 Build and Flash"
    printf "║    %-58s ║\n" "Compiles and flashes the firmware onto the device."
    echo "║                                                               ║"
    printf "║ %-63s ║\n" "2) 🔨 Build Only"
    printf "║    %-58s ║\n" "Compiles the firmware but does NOT flash it."
    echo "║                                                               ║"
    printf "║ %-63s ║\n" "3) 📡 Flash Only"
    printf "║    %-58s ║\n" "Flashes an already compiled firmware onto the device."
    printf "║    %-58s ║\n" "Build firmware can be found in src/bin/."
    echo "║                                                               ║"
    printf "║ %-64s ║\n" "4) 🖥 Open Terminal"
    printf "║    %-58s ║\n" "Opens a terminal for serial debugging via 'make term'."
    echo "║                                                               ║"
    printf "║ %-63s ║\n" "5) 🔍 Force Re-run of System Checks"
    printf "║    %-58s ║\n" "Re-run system checks with the next execution."
    printf "║    %-58s ║\n" "Checks required packages, interfaces, configurations."
    printf "║    %-58s ║\n" "Unlocks the nRF device and updates chat ids."
    echo "║                                                               ║"
    printf "║ %-66s ║\n" "6) ⚙️  Modify Environment Variables"
    printf "║    %-58s ║\n" "Change BOARD, ENABLE_CONSOLE_THREAD, or VERBOSE mode."
    echo "║                                                               ║"
    printf "║ %-66s ║\n" "7) ℹ️  Help Page"
    printf "║    %-58s ║\n" "Shows this help page explaining all options."
    echo "║                                                               ║"
    printf "║ %-62s ║\n" "8) ❌ Exit"
    printf "║    %-58s ║\n" "Closes the script and exits."
    echo "╚═══════════════════════════════════════════════════════════════╝"
    echo ""

    return
}

################################################################################
############################## TERMINAL FUNCTIONS ##############################
################################################################################

# Function to run a command in a new terminal window
run_in_terminal() {
    gnome-terminal -- bash -c "$1; exec bash"
}

# Function to run a command in the background (non-blocking)
run_in_background() {
    TIMESTAMP=$(date +"%Y-%m-%d %H:%M:%S")
    if [[ $VERBOSE -eq 1 ]]; then
        echo "[$TIMESTAMP] Executing: $1"
        eval "$1"
    else
        echo "===================================================" >> "$LOG_FILE"
        echo "[$TIMESTAMP] Executing: $1" >> "$LOG_FILE"
        eval "$1" >> "$LOG_FILE" 2>&1 &
    fi
}

################################################################################
################################ SYSTEM CHECKS #################################
################################################################################

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

# Function to check if the `tap0` interface is up
check_interface() {
    echo "🔍 Checking if network interface $INTERFACE is up..."

    if ip link show $INTERFACE &> /dev/null; then
        echo "✅ Interface $INTERFACE is up and running."
    else
        echo "❌ Interface $INTERFACE is DOWN!"
        echo "🔧 Run: sudo ip tuntap add dev tap0 mode tap user $(whoami) && sudo ip link set tap0 up"
        exit 1
    fi
}

# Function to check if `config.ini` exists
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
chat_ids = $CHAT_IDS
EOL

        echo "✅ Config file created at: $CONFIG_FILE"
    fi
}

# Function to check if the nRF device is locked
check_if_locked() {
    OUTPUT=$(openocd -c 'interface jlink; transport select swd; source [find target/nrf52.cfg]' \
                      -c 'init' -c 'mdw 0x10000000' -c 'shutdown' 2>&1)

    if echo "$OUTPUT" | grep -q "cannot access"; then
        echo "🔒 nRF device is locked."
        return 1  # Locked
    else
        echo "🔓 nRF device already unlocked."
        return 0  # Unlocked
    fi
}

# Function to unlock the nRF device
unlock_nrf_device() {
    echo "🔍 Checking if nRF device is locked..."
    check_if_locked
    LOCKED=$?

    if [[ "$LOCKED" -eq 1 ]]; then
        echo "🔑 Unlocking nRF device..."
        run_in_background "openocd -c 'interface jlink; transport select swd; source [find target/nrf52.cfg]' \
                -c 'init' -c 'nrf52_recover' -c 'shutdown'"
        sleep 3
        echo "🔓 nRF device unlocked."
    fi
}

# Function to update chat IDs
update_chat_ids() {
    echo "🚀 Updating chat IDs..."
    python3 "$UPDATE_SCRIPT"
    if [ $? -ne 0 ]; then
        echo "❌ Error updating chat IDs! Exiting..."
        exit 1
    fi
}

# Function to run system checks
system_checks() {
    if [[ -f "$CHECKS_DONE_FILE" ]]; then
        echo "⚡ Skipping system checks (already completed in this session)."
        return
    fi

    check_packages;
    check_interface;
    check_config_file;
    update_chat_ids;
    unlock_nrf_device;
    echo "✅ All system checks completed!"
}

################################################################################
################################# SCRIPT CONFIG ################################
################################################################################

# Function to load settings from config.ini
load_config() {
    if [[ -f "$CONFIG_FILE" ]]; then
        BOARD=$(awk -F '=' '/^board/ {print $2}' "$CONFIG_FILE" | xargs)
        ENABLE_CONSOLE_THREAD=$(awk -F '=' '/^enable_console_thread/ {print $2}' "$CONFIG_FILE" | xargs)
        VERBOSE=$(awk -F '=' '/^verbose/ {print $2}' "$CONFIG_FILE" | xargs)

        # Fallback to defaults if config is malformed
        [[ -z "$BOARD" ]] && BOARD="nrf52dk"
        [[ -z "$ENABLE_CONSOLE_THREAD" ]] && ENABLE_CONSOLE_THREAD=0
        [[ -z "$VERBOSE" ]] && VERBOSE=0
    else
        echo "⚠️ Config file not found! Creating default config..."
        save_config  # Creates a new config.ini file with defaults
    fi
}

# Function to save settings to config.ini
save_config() {
    if [[ -f "$CONFIG_FILE" ]]; then
        # Create a temp file to store modified settings
        awk -v board="$BOARD" \
            -v enable_console_thread="$ENABLE_CONSOLE_THREAD" \
            -v verbose="$VERBOSE" \
            '
            BEGIN { in_settings = 0 }
            /^\[settings\]/ { in_settings = 1; print; next }
            /^\[/ && !/^\[settings\]/ { in_settings = 0 }
            in_settings && /board *=/ { print "board = " board; next }
            in_settings && /enable_console_thread *=/ { print "enable_console_thread = " enable_console_thread; next }
            in_settings && /verbose *=/ { print "verbose = " verbose; next }
            { print }
            ' "$CONFIG_FILE" > "$CONFIG_FILE.tmp"

        mv "$CONFIG_FILE.tmp" "$CONFIG_FILE"
    else
        echo "⚠️ Config file not found! Creating default config..."
        cat <<EOL > "$CONFIG_FILE"
[settings]
board = $BOARD
enable_console_thread = $ENABLE_CONSOLE_THREAD
verbose = $VERBOSE
EOL
    fi

    echo "✅ Config updated in $CONFIG_FILE"
}

################################################################################
############################### APPLICATION LOGIC ##############################
################################################################################

# Function to build firmware
build_firmware() {
    echo "🔨 Building firmware for BOARD=$BOARD..."

    if [[ $VERBOSE -eq 1 ]]; then
        echo "⏳ Running build process with live output..."
        make clean all BOARD=$BOARD ENABLE_CONSOLE_THREAD=$ENABLE_CONSOLE_THREAD
    else
        run_in_background "make clean all BOARD=$BOARD ENABLE_CONSOLE_THREAD=$ENABLE_CONSOLE_THREAD"
        BUILD_PID=$!  # Capture the process ID (PID) of the background build
        echo "✅ Build started in background. Check $LOG_FILE for progress."
    fi
}

# Function to flash firmware
flash_firmware() {
    echo "🚀 Flashing firmware..."
    run_in_background "make flash BOARD=$BOARD"
    if [[ $VERBOSE -ne 1 ]]; then
        echo "✅ Flash process started in background. Check $LOG_FILE for details."
    fi
}

# Function to build and flash firmware
build_and_flash() {
    build_firmware

    # If VERBOSE is off
    if [[ $VERBOSE -eq 0 && -n "$BUILD_PID" ]]; then
        echo "⏳ Waiting for build process to finish..."
        wait "$BUILD_PID" # Wait for the build process to complete
    fi

    flash_firmware
}

# Function to open the RIOT-OS terminal
open_terminal() {
    echo "🖥️ Opening RIOT-OS terminal..."
    run_in_terminal "make term BOARD=$BOARD"
}

# Function to reset system checks
reset_checks() {
    rm -f "$CHECKS_DONE_FILE"
    echo "✅ System checks will run again on the next execution."
}

################################################################################
################################# VISUALIZATION ################################
################################################################################

# Function to modify environment variables
set_env_variables() {
    while true; do
        clear
        echo "╔═══════════════════════════════════════════════════════════════╗"
        echo "║                🔧 MODIFY ENVIRONMENT VARIABLES                ║"
        echo "╚═══════════════════════════════════════════════════════════════╝"
        echo ""

        # Display Current Environment Variables
        echo "╔═══════════════════════════════════════════════════════════════╗"
        printf "║  📌 BOARD: %-50s ║\n" "$BOARD"
        printf "║  ⚙️  ENABLE_CONSOLE_THREAD: %-34s ║\n" "$([[ $ENABLE_CONSOLE_THREAD -eq 1 ]] && echo "ENABLED" || echo "DISABLED")"
        printf "║  📝 VERBOSE MODE: %-43s ║\n" "$([[ $VERBOSE -eq 1 ]] && echo "ON (Live Output)" || echo "OFF (Silent Mode)")"
        echo "╚═══════════════════════════════════════════════════════════════╝"
        echo ""

        # Display Options
        echo "╔═══════════════════════════════════════════════════════════════╗"
        printf "║ %-64s ║\n" "1) 🛠 Change BOARD"
        printf "║ %-63s ║\n" "2) 🔄 Toggle ENABLE_CONSOLE_THREAD"
        printf "║ %-63s ║\n" "3) 📢 Toggle VERBOSE MODE"
        printf "║ %-66s ║\n" "4) ⬅️  Return to Main Menu"
        echo "╚═══════════════════════════════════════════════════════════════╝"
        echo ""

        read -p "🎯 Select an option: " env_choice
        BUILD_NEEDED=0  # Flag to check if a rebuild is needed

        case $env_choice in
            1)
                read -p "✏️  Enter new BOARD value (e.g., native, nrf52840dk): " new_board
                if [[ -n "$new_board" && "$new_board" != "$BOARD" ]]; then
                    BOARD="$new_board"
                    echo "✅ BOARD changed to $BOARD"
                    BUILD_NEEDED=1  # Mark build as needed
                else
                    echo "❌ No change. Keeping BOARD=$BOARD"
                fi
                ;;
            2)
                ENABLE_CONSOLE_THREAD=$((1 - ENABLE_CONSOLE_THREAD))
                echo "✅ ENABLE_CONSOLE_THREAD toggled to $([[ $ENABLE_CONSOLE_THREAD -eq 1 ]] && echo "ENABLED" || echo "DISABLED")"
                BUILD_NEEDED=1  # Mark build as needed
                ;;
            3)
                VERBOSE=$((1 - VERBOSE))
                echo "✅ VERBOSE MODE toggled to $([[ $VERBOSE -eq 1 ]] && echo "ON (Live Output)" || echo "OFF (Silent Mode)")"
                save_config
                ;;
            4)
                return
                ;;
            *)
                echo "❌ Invalid choice! Try again."
                ;;
        esac

        # If an environment variable changed, trigger a new build
        if [[ $BUILD_NEEDED -eq 1 ]]; then
            echo ""
            echo "🔄 Environment variables changed. Starting a new build..."
            reset_checks
            build_firmware
            echo "✅ Build completed!"
            save_config
        fi

        read -p "Press Enter to continue..."
    done
}

load_config

# Persistent Interactive Menu
while true; do
    clear
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

    # Display Environment Variables
    echo "╔═══════════════════════════════════════════════════════════════╗"
    printf "║  🌍 ENVIRONMENT SETTINGS                                      ║\n"
    echo "║---------------------------------------------------------------║"
    printf "║  📌 BOARD: %-50s ║\n" "$BOARD"
    printf "║  ⚙️  ENABLE_CONSOLE_THREAD: %-34s ║\n" "$([[ $ENABLE_CONSOLE_THREAD -eq 1 ]] && echo "ENABLED" || echo "DISABLED")"
    printf "║  📝 VERBOSE MODE: %-43s ║\n" "$([[ $VERBOSE -eq 1 ]] && echo "ON (Live Output)" || echo "OFF (Silent Mode)")"
    echo "╚═══════════════════════════════════════════════════════════════╝"

    # Menu Options
    echo "╔═══════════════════════════════════════════════════════════════╗"
    printf "║ %-63s ║\n" "1) 🚀 Build and Flash"
    printf "║ %-63s ║\n" "2) 🔨 Build Only"
    printf "║ %-63s ║\n" "3) 📡 Flash Only"
    printf "║ %-64s ║\n" "4) 🖥  Open Terminal"
    printf "║ %-66s ║\n" "5) ⚙️  Modify Environment Variables"
    printf "║ %-63s ║\n" "6) 🔍 Force Re-run of System Checks"
    printf "║ %-66s ║\n" "7) ℹ️  Help Page"
    printf "║ %-62s ║\n" "8) ❌ Exit"
    echo "╚═══════════════════════════════════════════════════════════════╝"
    echo ""

    # Process User Input
    read -p "🎯 Select an option: " choice

    case $choice in
        1) system_checks; build_and_flash ;;
        2) system_checks; build_firmware ;;
        3) system_checks; flash_firmware ;;
        4) open_terminal ;;
        5) set_env_variables ;;
        6) reset_checks ;;
        7) show_help ;;
        8) echo "👋 Exiting..."; exit 0 ;;
        *) echo "❌ Invalid choice! Try again." ;;
    esac
    read -p "Press Enter to continue..."
done
