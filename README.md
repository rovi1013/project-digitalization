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

## TODOs

- [ ] Which instant messaging protocoll shoudl we use?
- [ ] Which modules from RIOT-OS do we need?
- [ ] Create the project architecture.
- [ ] Present the project architecture

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
