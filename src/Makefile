# Name of the application
APPLICATION = project-digitalization

# If no BOARD is found in the environment, use this default:
BOARD ?= nrf52840dk

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../RIOT

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

####################################################################################################
############################ GET ENVIRONMENT VARIABLES FROM config.ini #############################
####################################################################################################

# Telegram variables
TELEGRAM_BOT_TOKEN := $(shell awk -F' = ' '/bot_token/ {print $$2}' config.ini)
TELEGRAM_CHAT_IDS := $(shell awk -F' = ' '/chat_ids/ {print $$2}' config.ini)
TELEGRAM_SERVER_URL := $(shell awk -F' = ' '/url/ {print $$2}' config.ini)
# CoAP Variables
COAP_SERVER_ADDRESS := $(shell awk -F' = ' '/address/ {print $$2}' config.ini)
COAP_SERVER_PORT := $(shell awk -F' = ' '/port/ {print $$2}' config.ini)
COAP_SERVER_URI_PATH := $(shell awk -F' = ' '/uri_path/ {print $$2}' config.ini)
# Additional Configurations
TEMPERATURE_NOTIFICATION_INTERVAL := $(shell awk -F' = ' '/temperature_notification_interval/ {print $$2}' config.ini)
ENABLE_CONSOLE_THREAD := $(shell awk -F' = ' '/enable_console_thread/ {print $$2}' config.ini)
ENABLE_LED_FEEDBACK := $(shell awk -F' = ' '/enable_led_feedback/ {print $$2}' config.ini)

# Pass variables as C macros
CFLAGS += -DTELEGRAM_BOT_TOKEN=\"$(TELEGRAM_BOT_TOKEN)\"
CFLAGS += -DTELEGRAM_CHAT_IDS=\"$(TELEGRAM_CHAT_IDS)\"
CFLAGS += -DTELEGRAM_SERVER_URL=\"$(TELEGRAM_SERVER_URL)\"
CFLAGS += -DCOAP_SERVER_ADDRESS=\"$(COAP_SERVER_ADDRESS)\"
CFLAGS += -DCOAP_SERVER_PORT=\"$(COAP_SERVER_PORT)\"
CFLAGS += -DCOAP_SERVER_URI_PATH=\"$(COAP_SERVER_URI_PATH)\"
CFLAGS += -DTEMPERATURE_NOTIFICATION_INTERVAL=$(TEMPERATURE_NOTIFICATION_INTERVAL)
CFLAGS += -DENABLE_CONSOLE_THREAD=$(ENABLE_CONSOLE_THREAD)
CFLAGS += -DENABLE_LED_FEEDBACK=$(ENABLE_LED_FEEDBACK)

# Increase default RIOT CoAP request size limit
CFLAGS += -DCONFIG_GCOAP_PDU_BUF_SIZE=512

####################################################################################################
###################################### MODULES CONFIGURATION #######################################
####################################################################################################

USEMODULE += ztimer

# Use SAUL module only for non-native boards
ifneq ($(BOARD),native)
USEMODULE += saul_default
USEMODULE += saul_reg
USEMODULE += saul_nrf_temperature
endif

############### SHELL & DEBUGGING ###############
# Only if shell is enabled
ifeq ($(ENABLE_CONSOLE_THREAD),1)
# Core shell functionality
USEMODULE += shell
USEMODULE += shell_cmds_default

# Process and network statistics
USEMODULE += ps
USEMODULE += netstats_l2
USEMODULE += netstats_ipv6
USEMODULE += netstats_rpl

# Debugging tools for networking
USEMODULE += gnrc_icmpv6_echo  # Enables ping (ICMPv6 echo requests)
USEMODULE += shell_cmd_gnrc_udp  # Allows UDP interaction via shell
endif

############### NETWORKING ###############
# Core IPv6 networking
USEMODULE += gnrc_ipv6
USEMODULE += gnrc_ipv6_router
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_netif

# UDP and Routing
USEMODULE += gnrc_udp
USEMODULE += gnrc_rpl

# Network Interface Management
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif

# ICMPv6
USEMODULE += gnrc_icmpv6_error

# DNS Resolution
USEMODULE += sock_dns
USEMODULE += gnrc_ipv6_nib_dns
USEMODULE += auto_init_sock_dns

############### COAP ###############
USEMODULE += gcoap
USEMODULE += netutils
USEMODULE += uri_parser

# Custom module with additional utilities
DIRS += utils
USEMODULE += custom_utils

####################################################################################################
#################################### APPLICATION CONFIGURATION #####################################
####################################################################################################

# Include path for utils folder
INCLUDES += -I$(CURDIR)/utils

# Source files
SRC += main.c
SRC += led_control.c
SRC += cmd_control.c
SRC += cpu_temperature.c
SRC += coap_post.c
SRC += configuration.c

# RIOT makefile
include $(RIOTBASE)/Makefile.include
