# Project Digitalization (WiSe 2024/35)

This is the repository for the digitalization project at the FRA-UAS. Using RIOT-OS to create a small application to control a *Nordic* nRF52840 (DK) device. Providing additional remote access via the telegram protocol.

## Modules

### [shell.h](https://doc.riot-os.org/group__sys__shell.html)

Simple shell interpreter used to parse arguments.

### [periph/gpio.h](https://doc.riot-os.org/group__drivers__periph__gpio.html)

Peripheral dirver to controll the specific pins on the board.

### [board.h](https://doc.riot-os.org/group__boards__nrf52840dk.html)

Specific configuration for the nRF52840 DK board, used to adress the LED pins.

## Useful commands

Flash the nRF52840 board:

```bash
BOARD=nrf52840dk make all flash term
```

List all availibe RIOT modules:

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

## TODOs

- [ ] Which instant messaging protocoll should we use?
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
