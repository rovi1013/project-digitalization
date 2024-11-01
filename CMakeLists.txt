cmake_minimum_required(VERSION 3.13)
project(project_digitalization C)

# List all source files here to enable code insight
set(SOURCE_FILES
        src/main.c
        src/telegram_bot.c
        src/telegram_bot.h
        src/led_control.c
        src/led_control.h
        src/token_manager.c
        src/token_manager.h
        src/cmd_control.c
        src/cmd_control.h
        src/message_handler.c
        src/message_handler.h
)

# Set RIOT OS base directory
set(RIOTBASE ./RIOT)

# Add the RIOT OS include directories
include_directories(
        ${RIOTBASE}/boards/nrf9160dk/include
        ${RIOTBASE}/drivers/include
        ${RIOTBASE}/sys/include
)

# Add a dummy executable target for CLion code insight (won't be used for building)
add_executable(dummy_target ${SOURCE_FILES})

# Add a custom target to run the Makefile
add_custom_target(build_all
        COMMAND make -C ${CMAKE_SOURCE_DIR}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
