#!/bin/bash

# Make sure this script is executed as sudo user
if [ "$EUID" -ne 0 ]; then
    echo "❌ This script must be run with sudo. Use: sudo bash build.sh"
    exit 1
fi

# Define paths
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CONFIG_FILE="$SCRIPT_DIR/src/config.ini"
UPDATE_SCRIPT="$SCRIPT_DIR/websocket/update_chat_ids.py"
CHECKS_DONE_FILE="/tmp/build_env_setup_done"

# Define requirements
REQUIRED_PACKAGES=("git" "gcc-arm-none-eabi" "make" "gcc-multilib" "libstdc++-arm-none-eabi-newlib" "openocd" "gdb-multiarch" "doxygen" "wget" "unzip" "python3-serial" "python3-venv" "python3-pip" "gnome-terminal")
INTERFACE="tap0"

# Default Environment Variables
BOARD="nrf52840dk"
TEMPERATURE_NOTIFICATION_INTERVAL=5
ENABLE_CONSOLE_THREAD=0
ENABLE_LED_FEEDBACK=0
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
    echo "the IoT application. (Logging: $LOG_FILE)"
    echo "Below are the available options:"

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
    printf "║    %-58s ║\n" "Opens a terminal, useful for debugging."
    echo "║                                                               ║"
    printf "║ %-63s ║\n" "5) 🔍 Force Re-run of System Checks"
    printf "║    %-58s ║\n" "Re-run system checks with the next execution."
    printf "║    %-58s ║\n" "Checks required packages, interfaces, configurations."
    printf "║    %-58s ║\n" "Unlocks the nRF device and updates chat ids."
    echo "║                                                               ║"
    printf "║ %-66s ║\n" "6) ⚙️  Modify Environment Variables"
    printf "║    %-58s ║\n" "BOARD: Choose the IoT board to run the application,"
    printf "║    %-58s ║\n" "TEMPERATURE_NOTIFICATION_INTERVAL: Set the interval of"
    printf "║    %-58s ║\n" "               temperature notifications (in minutes),"
    printf "║    %-58s ║\n" "ENABLE_CONSOLE_THREAD: Toggle the RIOT-OS console,"
    printf "║    %-58s ║\n" "ENABLE_LED_FEEDBACK: Toggle LED Feedback for CoAP,"
    printf "║    %-58s ║\n" "VERBOSE: Toggle the display of additional information."
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
        {
        echo "==================================================="
        echo "[$TIMESTAMP] Executing: $1"
        eval "$1"
        } | sudo tee -a "$LOG_FILE" >/dev/null &
    fi
}

################################################################################
################################ SYSTEM CHECKS #################################
################################################################################

# Function to check and install required packages
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
        echo "🔧 Installing missing packages..."

        if [[ $VERBOSE -eq 1 ]]; then
            sudo apt update && sudo apt install -y "${MISSING_PACKAGES[@]}"
        else
            echo "⏳ Installing missing packages in the background. Check $LOG_FILE for progress."
            sudo apt update -qq && sudo apt install -y "${MISSING_PACKAGES[@]}" | sudo tee -a "$LOG_FILE"
        fi

        sleep 3  # Give some time for installation to start

        if [ $? -ne 0 ]; then
            echo "❌ Failed to install some packages. Check $LOG_FILE for details."
            exit 1
        else
            echo "✅ All required packages installed successfully."
        fi
    else
        echo "✅ All required packages are installed."
    fi
}

# Function to check if the `tap0` interface is up
check_interface() {
    echo "🔍 Checking if network interface $INTERFACE is up..."

    if ip link show "$INTERFACE" &> /dev/null; then
        echo "✅ Interface $INTERFACE is up and running."
    else
        echo "❌ Interface $INTERFACE is DOWN!"
        echo "🔧 Setting up network interface $INTERFACE..."

        if [[ $VERBOSE -eq 1 ]]; then
            sudo ip tuntap add dev "$INTERFACE" mode tap user "$(whoami)"
            sudo ip link set "$INTERFACE" up
            sudo ip a a 2001:db8::1/48 dev "$INTERFACE"
            sudo ip r d 2001:db8::/48 dev "$INTERFACE"
            sudo ip r a 2001:db8::2 dev "$INTERFACE"
            sudo ip r a 2001:db8::/48 via 2001:db8::2 dev "$INTERFACE"
        else
            run_in_background "
                sudo ip tuntap add dev $INTERFACE mode tap user $(whoami);
                sudo ip link set $INTERFACE up;
                sudo ip a a 2001:db8::1/48 dev $INTERFACE;
                sudo ip r d 2001:db8::/48 dev $INTERFACE;
                sudo ip r a 2001:db8::2 dev $INTERFACE;
                sudo ip r a 2001:db8::/48 via 2001:db8::2 dev $INTERFACE;
            "
            echo "⏳ Network interface setup running in the background. Check $LOG_FILE for progress."
        fi

        sleep 3  # Allow time for the setup to initiate

        # Re-run the check
        if ip link show "$INTERFACE" &> /dev/null; then
            echo "✅ Interface $INTERFACE is now up."
        else
            echo "❌ Failed to bring up interface $INTERFACE. Check $LOG_FILE for details."
            exit 1
        fi
    fi
}

# Function to check if `config.ini` exists
check_config_file() {
    if [ -f "$CONFIG_FILE" ]; then
        echo "✅ Configuration file exists."
        return
    fi

    TEMPLATE_FILE="$SCRIPT_DIR/src/config.ini.template"

    if [ -f "$TEMPLATE_FILE" ]; then
        echo "⚠️ Missing config.ini file. Using config.ini.template..."
        mv "$TEMPLATE_FILE" "$CONFIG_FILE"
    else
        echo "⚠️ Missing config.ini file. Creating a new one with default values..."
        cat <<EOL > "$CONFIG_FILE"
[telegram]
bot_token = your_telegram_bot_token
chat_ids =
url = https://api.telegram.org/bot

[websocket]
address = 2001:470:7347:c810::1234
port = 5683
uri_path = /message

[settings]
board = nrf52840dk
temperature_notification_interval = 5
enable_console_thread = 0
enable_led_feedback = 0
verbose = 0
EOL
        echo "✅ Created a new config.ini file at $CONFIG_FILE."
    fi

    # Prompt for bot token
    while true; do
       read -rp "Enter your Telegram bot token (or type 'exit' to quit): " BOT_TOKEN
       if [[ "$BOT_TOKEN" == "exit" ]]; then
           echo "❌ Exiting due to missing Telegram bot token."
           exit 1
       elif [[ -n "$BOT_TOKEN" ]]; then
           break
       fi
       echo "⚠️ Bot token cannot be empty!"
   done

    # Modify the new config.ini file
    sed -i "s|bot_token = your_telegram_bot_token|bot_token = $BOT_TOKEN|g" "$CONFIG_FILE"

    echo "✅ Configuration file initialized successfully!"
    echo "📄 Updated $CONFIG_FILE with your Telegram bot token."
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
        echo "❌ Error updating chat IDs!"
        echo "📄 Please update the $CONFIG_FILE with first names and chat_ids manually."
        echo "[telegram]"
        echo "bot_token = ..."
        echo "chat_ids = peter:73917876234,max:2985678432,..."
        echo ""
        echo "Press Enter after updating to continue..."
        read -r  # Wait for user confirmation
        return
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
        TEMPERATURE_NOTIFICATION_INTERVAL=$(awk -F '=' '/^temperature_notification_interval/ {print $2}' "$CONFIG_FILE" | xargs)
        ENABLE_CONSOLE_THREAD=$(awk -F '=' '/^enable_console_thread/ {print $2}' "$CONFIG_FILE" | xargs)
        ENABLE_LED_FEEDBACK=$(awk -F '=' '/^enable_led_feedback/ {print $2}' "$CONFIG_FILE" | xargs)
        VERBOSE=$(awk -F '=' '/^verbose/ {print $2}' "$CONFIG_FILE" | xargs)

        # Fallback to defaults if config is malformed
        [[ -z "$BOARD" ]] && BOARD="nrf52840dk"
        [[ -z "$TEMPERATURE_NOTIFICATION_INTERVAL" ]] && TEMPERATURE_NOTIFICATION_INTERVAL=5
        [[ -z "$ENABLE_CONSOLE_THREAD" ]] && ENABLE_CONSOLE_THREAD=0
        [[ -z "$ENABLE_LED_FEEDBACK" ]] && ENABLE_LED_FEEDBACK=0
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
            -v temperature_notification_interval="$TEMPERATURE_NOTIFICATION_INTERVAL" \
            -v enable_console_thread="$ENABLE_CONSOLE_THREAD" \
            -v enable_led_feedback="$ENABLE_LED_FEEDBACK" \
            -v verbose="$VERBOSE" \
            '
            BEGIN { in_settings = 0 }
            /^\[settings\]/ { in_settings = 1; print; next }
            /^\[/ && !/^\[settings\]/ { in_settings = 0 }
            in_settings && /board *=/ { print "board = " board; next }
            in_settings && /temperature_notification_interval *=/ { print "temperature_notification_interval = " temperature_notification_interval; next }
            in_settings && /enable_console_thread *=/ { print "enable_console_thread = " enable_console_thread; next }
            in_settings && /enable_led_feedback *=/ { print "enable_led_feedback = " enable_led_feedback; next }
            in_settings && /verbose *=/ { print "verbose = " verbose; next }
            { print }
            ' "$CONFIG_FILE" > "$CONFIG_FILE.tmp"

        mv "$CONFIG_FILE.tmp" "$CONFIG_FILE"
    else
        echo "⚠️ Config file not found! Creating default config..."
        cat <<EOL > "$CONFIG_FILE"
[settings]
board = $BOARD
temperature_notification_interval = $TEMPERATURE_NOTIFICATION_INTERVAL
enable_console_thread = $ENABLE_CONSOLE_THREAD
enable_led_feedback = $ENABLE_LED_FEEDBACK
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
        make clean all BOARD="$BOARD" ENABLE_CONSOLE_THREAD="$ENABLE_CONSOLE_THREAD"
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
        printf "║  🛠  BOARD: %-50s ║\n" "$BOARD"
        printf "║  ⏲️  TEMPERATURE_NOTIFICATION_INTERVAL: %-22s ║\n" "${TEMPERATURE_NOTIFICATION_INTERVAL} min"
        printf "║  ⚙️  ENABLE_CONSOLE_THREAD: %-34s ║\n" "$([[ $ENABLE_CONSOLE_THREAD -eq 1 ]] && echo "ENABLED" || echo "DISABLED")"
        printf "║  💡 ENABLE_LED_FEEDBACK: %-36s ║\n" "$([[ ENABLE_LED_FEEDBACK -eq 1 ]] && echo "ENABLED" || echo "DISABLED")"
        printf "║  📢 VERBOSE MODE: %-43s ║\n" "$([[ $VERBOSE -eq 1 ]] && echo "ON (Live Output)" || echo "OFF (Silent Mode)")"
        echo "╚═══════════════════════════════════════════════════════════════╝"
        echo ""

        # Display Options
        echo "╔═══════════════════════════════════════════════════════════════╗"
        printf "║ %-64s ║\n" "1) 🛠  Change BOARD"
        printf "║ %-66s ║\n" "2) ⏲️  Change TEMPERATURE_NOTIFICATION_INTERVAL"
        printf "║ %-66s ║\n" "3) ⚙️  Toggle ENABLE_CONSOLE_THREAD"
        printf "║ %-63s ║\n" "4) 💡 Toggle ENABLE_LED_FEEDBACK"
        printf "║ %-63s ║\n" "5) 📢 Toggle VERBOSE MODE"
        printf "║ %-66s ║\n" "6) ⬅️  Return to Main Menu"
        echo "╚═══════════════════════════════════════════════════════════════╝"
        echo ""

        read -p "🎯 Select an option: " env_choice
        REBUILD_NEEDED=0  # Flag to check if a rebuild is needed

        case $env_choice in
            1)
                read -p "✏️  Enter new BOARD value (e.g., native, nrf52840dk): " new_board
                if [[ -n "$new_board" && "$new_board" != "$BOARD" ]]; then
                    BOARD="$new_board"
                    echo "✅ BOARD changed to $BOARD"
                    REBUILD_NEEDED=1
                else
                    echo "❌ No change. Keeping BOARD=$BOARD"
                fi
                ;;
            2)
                read -p "✏️  Enter new TEMPERATURE_NOTIFICATION_INTERVAL value (in minutes): " new_interval
                if [[ -n "$new_interval" && "$new_interval" != "$TEMPERATURE_NOTIFICATION_INTERVAL" ]]; then
                    TEMPERATURE_NOTIFICATION_INTERVAL="$new_interval"
                    echo "✅ TEMPERATURE_NOTIFICATION_INTERVAL changed to $TEMPERATURE_NOTIFICATION_INTERVAL"
                    REBUILD_NEEDED=1
                else
                    echo "❌ No change. Keeping TEMPERATURE_NOTIFICATION_INTERVAL=$TEMPERATURE_NOTIFICATION_INTERVAL"
                fi
                ;;
            3)
                ENABLE_CONSOLE_THREAD=$((1 - ENABLE_CONSOLE_THREAD))
                echo "✅ ENABLE_CONSOLE_THREAD toggled to $([[ $ENABLE_CONSOLE_THREAD -eq 1 ]] && echo "ENABLED" || echo "DISABLED")"
                REBUILD_NEEDED=1
                ;;
            4)
                ENABLE_LED_FEEDBACK=$((1 - ENABLE_LED_FEEDBACK))
                echo "✅ ENABLE_LED_FEEDBACK toggled to $([[ ENABLE_LED_FEEDBACK -eq 1 ]] && echo "ENABLED" || echo "DISABLED")"
                REBUILD_NEEDED=1
                ;;
            5)
                VERBOSE=$((1 - VERBOSE))
                echo "✅ VERBOSE MODE toggled to $([[ $VERBOSE -eq 1 ]] && echo "ON (Live Output)" || echo "OFF (Silent Mode)")"
                save_config
                ;;
            6)
                return
                ;;
            *)
                echo "❌ Invalid choice! Try again."
                ;;
        esac

        # If an environment variable changed, trigger a new build
        if [[ $REBUILD_NEEDED -eq 1 ]]; then
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
    printf "║  🛠  BOARD: %-50s ║\n" "$BOARD"
    printf "║  ⏲️  TEMPERATURE_NOTIFICATION_INTERVAL: %-22s ║\n" "${TEMPERATURE_NOTIFICATION_INTERVAL} min"
    printf "║  ⚙️  ENABLE_CONSOLE_THREAD: %-34s ║\n" "$([[ $ENABLE_CONSOLE_THREAD -eq 1 ]] && echo "ENABLED" || echo "DISABLED")"
    printf "║  💡 ENABLE_LED_FEEDBACK: %-36s ║\n" "$([[ ENABLE_LED_FEEDBACK -eq 1 ]] && echo "ENABLED" || echo "DISABLED")"
    printf "║  📢 VERBOSE MODE: %-43s ║\n" "$([[ $VERBOSE -eq 1 ]] && echo "ON (Live Output)" || echo "OFF (Silent Mode)")"
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
        4) system_checks; open_terminal ;;
        5) set_env_variables ;;
        6) reset_checks ;;
        7) show_help ;;
        8) echo "👋 Exiting..."; exit 0 ;;
        *) echo "❌ Invalid choice! Try again." ;;
    esac
    read -p "Press Enter to continue..."
done
