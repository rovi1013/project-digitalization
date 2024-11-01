# name of your application
APPLICATION = project-digitalization

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/RIOT/

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# RIOT modules
USEMODULE += shell
#USEMODULE += shell_commands
USEMODULE += xtimer
USEMODULE += periph_gpio


include $(RIOTBASE)/Makefile.include
