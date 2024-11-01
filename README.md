# Project Digitalization (WiSe 2024/35)

This is the repository for the digitalization project at the FRA-UAS. Using RIOT-OS to create a small application to 
control a _Nordic_ nRF52840 (DK) device. Providing additional remote access via the _Telegram_ bot API. 


## Project Structure

```bash
project/digitalization
├── Makefile                      # Main Makefile
├── .env                          # Secrets storage file
├── src/
│   ├── main.c                    # Main application file
│   ├── led_control.c             # LED control source
│   ├── led_control.h             # LED control header
│   ├── telegram_bot.c            # Telegram bot source
│   ├── telegram_bot.h            # Telegram bot header
│   └── further classes           # ...
└── README.md                     # Documentation

```


## RIOT-OS Modules

### [periph/gpio.h](https://doc.riot-os.org/group__drivers__periph__gpio.html)

Peripheral driver to control the specific pins on the board.

### [board.h](https://doc.riot-os.org/group__boards__nrf52840dk.html)

Specific configuration for the nRF52840 DK board, used to address the LED pins.

### [shell.h](https://doc.riot-os.org/group__sys__shell.html)

Simple shell interpreter used to parse arguments.

### [sock.h](https://doc.riot-os.org/group__net__sock__udp.html)

A network API for applications and libraries, used to create custom HTTP functionality with the UDP submodule.

### [jsmn.h](https://doc.riot-os.org/group__pkg__jsmn.html)

JSON parser library, used to process _Telegram_ bot messages.


## _Telegram_ Bot Integration

### Set Up a Bot

1. Open _Telegram_
2. Search for "BotFather" and start a new chat with it
3. Use ``/newbot`` to create a new bot
4. Use the provided token to access the HTTP API

The bot "BotFather" is used to create and manage bot accounts.

_Telegram_ Bot API [Documentation](https://core.telegram.org/bots/api)

### Bot Description

This bot is used to send and receive messages to and from a small (proof-of-concept) IoT device. These messages can be 
used to control the device running on RIOT-OS. See "Commands" for a list of all available controls.

### Bot Commands

...


## Useful commands

Flash the nRF52840 board:

```bash
BOARD=nrf52840dk make all flash term
```

List all available RIOT modules:

```bash
make info-modules
```


## Ask the Prof

### Module ``shell_commands``

[Makefile](./Makefile) module ``shell_commands`` error:

```bash
Error - using unknown modules: shell_commands
make: *** [/home/vincent/Workspace/project-digitalization/RIOT//Makefile.include:742: ..module-check] Error 1
```

Despite ``make info-modules`` showing that ``shell_commands`` is available. And the module is working in ``Tutorials/task-01`` on the same machine.

### Use of "jsmn" library

The jsmn library (parse JSON) is not included in RIOT OS despite it being mentioned in the [documentation](https://doc.riot-os.org/group__pkg__jsmn.html).


## TODOs

- [ ] Which instant messaging protocol should we use?
- [ ] Which modules from RIOT-OS do we need?
- [ ] Create the project architecture.
- [ ] Develop the application.
- [ ] Present the project.


## Timeline

### 2024-11-25: Architecture

- Submission and presentation of your architecture

### 2025-01-20: Demo

- Present your walking skeleton (incl. demo)

### 2025-02-10: Presentation

- Give a short presentation on your work (live demo?)

### 2025-02-21: Submission

- Final version of the code is in the repository
- You have granted access to me
- Send me your documentation
