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

# Extract values from config.ini
TELEGRAM_BOT_TOKEN := $(shell awk -F' = ' '/bot_token/ {print $$2}' config.ini)
CHAT_IDS := $(shell awk -F' = ' '/chat_ids/ {print $$2}' config.ini | tr ',' ' ')
URL := $(shell awk -F' = ' '/url/ {print $$2}' config.ini)
ADDRESS := $(shell awk -F' = ' '/address/ {print $$2}' config.ini)
PORT := $(shell awk -F' = ' '/port/ {print $$2}' config.ini)
ENDPOINT := $(shell awk -F' = ' '/endpoint/ {print $$2}' config.ini)

# Pass values as C macros
CFLAGS += -DTELEGRAM_BOT_TOKEN=\"$(TELEGRAM_BOT_TOKEN)\"
CFLAGS += -DCHAT_IDS=\"$(CHAT_IDS)\"
CFLAGS += -DURL=\"$(URL)\"
CFLAGS += -DADDRESS\"$(ADDRESS)\"
CFLAGS += -DPORT\"$(PORT)\"
CFLAGS += -DENDPOINT\"$(EDNPOINT)\"

# RIOT modules
USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += saul_default
USEMODULE += saul_reg
USEMODULE += periph_gpio
USEMODULE += ztimer
USEMODULE += printf_float
#USEPKG += jsmn

# Use module only for non-native boards
ifneq ($(BOARD),native)
USEMODULE += saul_nrf_temperature
endif


########################################
################ ADJUST ################
########################################

############## NETWORKING ##############
# RIOT networking modules
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
# Activate ICMPv6 error messages
USEMODULE += gnrc_icmpv6_error
# Specify the mandatory networking module for a IPv6 routing node
USEMODULE += gnrc_ipv6_default
# Add a routing protocol
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo
USEMODULE += gnrc_dhcpv6_client
USEMODULE += gnrc_dhcpv6_client_simple_pd
USEMODULE += dhcpv6_client_ia_na
USEMODULE += dhcpv6_client_ia_pd
USEMODULE += shell_cmd_gnrc_udp
# Add also the shell, some shell commands
USEMODULE += ps
USEMODULE += netstats_l2
USEMODULE += netstats_ipv6
USEMODULE += netstats_rpl

# Optionally include DNS support. This includes resolution of names at an
# upstream DNS server and the handling of RDNSS options in Router Advertisements
# to auto-configure that upstream DNS server.
USEMODULE += sock_dns              # include DNS client
USEMODULE += gnrc_ipv6_nib_dns     # include RDNSS option handling
USEMODULE += auto_init_sock_dns

################# COAP #################
USEMODULE += gcoap
USEMODULE += netutils
USEMODULE += uri_parser
USEMODULE += ipv4_addr

########################################
################ ADJUST ################
########################################

# Our custom module(s)
DIRS += utils
USEMODULE += custom_utils

# Include path for utils folder
INCLUDES += -I$(CURDIR)/utils

# Source files
SRC += main.c
SRC += led_control.c
SRC += cmd_control.c
SRC += cpu_temperature.c
SRC += coap_control.c

# RIOT makefile
include $(RIOTBASE)/Makefile.include
