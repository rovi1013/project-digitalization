# Project Digitalization (WiSe 2024/35)

This is the repository for the digitalization project at the FRA-UAS. Using RIOT-OS to create a small application to control a *Nordic* nRF52840 (DK) device. Providing additional remote access via the telegram protocol.


## Useful commands

Flash the nRF52840 board:
```
BOARD=nrf52840dk make all flash term
```

List all availibe RIOT modules:
```
make info-modules
```

