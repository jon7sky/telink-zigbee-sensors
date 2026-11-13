# Qingping CGG1N

Battery Zigbee temperature and humidity sensor with an e-ink display.

Upstream description: 2022

> This page was generated from the board header and the build scripts. It has
> not been checked against the hardware.

## Summary

| | |
|---|---|
| Joins Zigbee as | `Qingping` / `CGG1N-z` |
| Build name | `ZCGG1N` |
| Board define | `BOARD_CGG1N` (id 7) |
| Board header | [src/board_cgg1n.h](../src/board_cgg1n.h) |
| Chip (header comment) | TLSR8253F512ET32 |
| Battery | CR2032 |
| Display | E-ink, driver [src/epd_cgg1n.c](../src/epd_cgg1n.c) |
| Temperature / humidity sensor | SHT4x, SHTC3, SHT30 (drivers enabled in this build) |
| Hardware notes (upstream) | <https://pvvx.github.io/CGG1_2022>, <https://pvvx.github.io/CGG1N> |

## Pins

Pin assignments the firmware uses for this build:

| Function | Pin |
|---|---|
| Button | PC4 |
| I2C (hardware) | PC0 and PC1 |
| E-ink display | SHD PA1, BUSY PD4, RST PA0, CSB PD2, SDA PB7, SCL PD7 |

Notes from the board header. Where they disagree with the table above, the table is what the firmware uses:

```
 https://pvvx.github.io/CGG1N

TLSR8253F512ET32

GPIO_PA0 - used EPD_RST
GPIO_PA1 - used EPD_SHD
GPIO_PA7 - SWS, free, (debug TX)
GPIO_PB1 - free
GPIO_PB4 - free
GPIO_PB5 - free (ADC1)
GPIO_PB6 - free (ADC2)
GPIO_PB7 - used EPD_SDA
GPIO_PC0 - SDA, used I2C
GPIO_PC1 - SCL, used I2C
GPIO_PC2 - TX, free, (Trigger, Output)
GPIO_PC3 - RX, free, (RDS)
GPIO_PC4 - used KEY
GPIO_PD2 - used EPD_CSB
GPIO_PD3 - free
GPIO_PD4 - used EPD_BUSY
GPIO_PD7 - used EPD_SCL
```

## Zigbee clusters

Set by `DEV_SERVICES` in the board header and [src/app_cfg.h](../src/app_cfg.h):

- Basic (`0x0000`), Power Configuration (`0x0001`), Poll Control (`0x0020`), OTA Upgrade (`0x0019`)
- Temperature Measurement (`0x0402`), Relative Humidity Measurement (`0x0405`), Thermostat User Interface Configuration (`0x0204`)

Button actions, settings and default reporting are the same for every board and are described in the [main README](../README.md).

## Build

Zigbee only:

```
make -s -j VERSION_BIN=_v0140 PROJECT_NAME=ZCGG1N POJECT_DEF="-DBOARD=BOARD_CGG1N" ZNAME="Qingping:CGG1N-z"
```

Zigbee with BLE OTA (see [ZBdevices.md](../ZBdevices.md)):

```
make -s -j VERSION_BIN=_zb0018 PROJECT_NAME=ZCGG1N USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_CGG1N" ZNAME="Qingping:CGG1N-z"
```

## Prebuilt firmware

- Zigbee only, for a hardware programmer: [bin/ZCGG1N_v0140.bin](../bin/ZCGG1N_v0140.bin)
- Zigbee only, OTA image: [bin/1141-0207-01403001-ZCGG1N.zigbee](../bin/1141-0207-01403001-ZCGG1N.zigbee)
- Zigbee with BLE OTA, for a hardware programmer: [bin_zb/ZCGG1N_zb0018.bin](../bin_zb/ZCGG1N_zb0018.bin)
- Zigbee with BLE OTA, OTA image: [bin_zb/1141-0207-00183001-ZCGG1N.zigbee](../bin_zb/1141-0207-00183001-ZCGG1N.zigbee)

[bin/README.md](../bin/README.md) explains the file names, including the images that convert a device from its stock firmware.
