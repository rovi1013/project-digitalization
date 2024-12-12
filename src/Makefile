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
USEMODULE += saul_default
USEMODULE += saul_reg
USEMODULE += saul_nrf_temperature
USEMODULE += periph_gpio
#USEPKG += jsmn

# Source files
SRC += led_control.c
SRC += cmd_control.c
SRC += sensor_mock.c
SRC += main.c

# RIOT makefile
include $(RIOTBASE)/Makefile.include
