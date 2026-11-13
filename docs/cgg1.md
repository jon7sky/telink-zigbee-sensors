# Qingping CGG1

Battery Zigbee temperature and humidity sensor with an e-ink display.

> This page was generated from the board header and the build scripts. It has
> not been checked against the hardware.

## Summary

| | |
|---|---|
| Joins Zigbee as | `Qingping` / `CGG1-z` |
| Build name | `ZCGG1` |
| Board define | `BOARD_CGG1` (id 2) |
| Board header | [src/board_cgg1.h](../src/board_cgg1.h) |
| Chip (header comment) | TLSR8253F512ET32 |
| Battery | CR2430 |
| Display | E-ink, driver [src/epd_cgg1.c](../src/epd_cgg1.c) |
| Temperature / humidity sensor | SHT4x, SHTC3, SHT30 (drivers enabled in this build) |
| Hardware notes (upstream) | <https://pvvx.github.io/CGG1> |

## Pins

Pin assignments the firmware uses for this build:

| Function | Pin |
|---|---|
| Button | PC4 |
| I2C (hardware) | PC0 and PC1 |
| E-ink display | SHD PA1, BUSY PD4, RST PA0, CSB PD2, SDA PB7, SCL PD7 |

Notes from the board header. Where they disagree with the table above, the table is what the firmware uses:

```
 https://pvvx.github.io/CGG1

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

EPD: https://pvvx.github.io/CGG1
```

## Zigbee clusters

Set by `DEV_SERVICES` in the board header and [src/app_cfg.h](../src/app_cfg.h):

- Basic (`0x0000`), Power Configuration (`0x0001`), Poll Control (`0x0020`), OTA Upgrade (`0x0019`)
- Temperature Measurement (`0x0402`), Relative Humidity Measurement (`0x0405`), Thermostat User Interface Configuration (`0x0204`)

Button actions, settings and default reporting are the same for every board and are described in the [main README](../README.md).

## Build

Zigbee only:

```
make -s -j VERSION_BIN=_v0140 PROJECT_NAME=ZCGG1 POJECT_DEF="-DBOARD=BOARD_CGG1" ZNAME="Qingping:CGG1-z"
```

Zigbee with BLE OTA (see [ZBdevices.md](../ZBdevices.md)):

```
make -s -j VERSION_BIN=_zb0018 PROJECT_NAME=ZCGG1 USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_CGG1" ZNAME="Qingping:CGG1-z"
```

## Prebuilt firmware

- Zigbee only, for a hardware programmer: [bin/ZCGG1_v0140.bin](../bin/ZCGG1_v0140.bin)
- Zigbee only, OTA image: [bin/1141-0202-01403001-ZCGG1.zigbee](../bin/1141-0202-01403001-ZCGG1.zigbee)
- Zigbee with BLE OTA, for a hardware programmer: [bin_zb/ZCGG1_zb0018.bin](../bin_zb/ZCGG1_zb0018.bin)
- Zigbee with BLE OTA, OTA image: [bin_zb/1141-0202-00183001-ZCGG1.zigbee](../bin_zb/1141-0202-00183001-ZCGG1.zigbee)

[bin/README.md](../bin/README.md) explains the file names, including the images that convert a device from its stock firmware.
