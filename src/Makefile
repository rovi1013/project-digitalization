# name of your application
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

# RIOT modules
USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += periph_gpio
#USEPKG += jsmn

# Source files
SRC += main.c
SRC += led_control.c
SRC += cmd_control.c

# RIOT makefile
include $(RIOTBASE)/Makefile.include
