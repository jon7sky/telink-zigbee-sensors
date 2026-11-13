# Tuya LKTMZL02

Battery Zigbee temperature and humidity sensor with an LCD.

Upstream description: ZigBee LKTMZL02 LCD

> This page was generated from the board header and the build scripts. It has
> not been checked against the hardware.

## Summary

| | |
|---|---|
| Joins Zigbee as | `Tuya` / `LKTMZL02-z` |
| Build name | `LKTMZL02Z` |
| Board define | `BOARD_LKTMZL02` (id 31) |
| Board header | [src/board_lktmzl02.h](../src/board_lktmzl02.h) |
| Chip (header comment) | TLSR8258 |
| Battery | 2 x AAA |
| Display | LCD, driver [src/lcd_lktmzl02.c](../src/lcd_lktmzl02.c) |
| Temperature / humidity sensor | CHT8215, AHT20/AHT30 (drivers enabled in this build) |
| Hardware notes (upstream) | <https://pvvx.github.io/LKTMZL02> |

## Pins

Pin assignments the firmware uses for this build:

| Function | Pin |
|---|---|
| Button | PC0 |
| LED | PB4 |
| I2C (hardware) | PC2 and PC3 |
| I2C for the display | SCL PC4, SDA PC1 |

Notes from the board header. Where they disagree with the table above, the table is what the firmware uses:

```
 https://pvvx.github.io/LKTMZL02

TLSR8258

GPIO_PA0 - free (Reed Switch, input)
GPIO_PA1 - free
GPIO_PA7 - SWS, (debug TX)
GPIO_PB1 - free
GPIO_PB4 - LED - "1" On
GPIO_PB5 - free, (TRG)
GPIO_PB6 - free
GPIO_PB7 - free
GPIO_PC0 - KEY
GPIO_PC1 - SDA1, used I2C LCD
GPIO_PC2 - SDA, used I2C Sensor
GPIO_PC3 - SCL, used I2C Sensor
GPIO_PC4 - SCL1, used I2C LCD
GPIO_PD2 - free
GPIO_PD3 - free
GPIO_PD4 - free
GPIO_PD7 - free
```

## Zigbee clusters

Set by `DEV_SERVICES` in the board header and [src/app_cfg.h](../src/app_cfg.h):

- Basic (`0x0000`), Power Configuration (`0x0001`), Poll Control (`0x0020`), OTA Upgrade (`0x0019`)
- Temperature Measurement (`0x0402`), Relative Humidity Measurement (`0x0405`), Thermostat User Interface Configuration (`0x0204`)

Button actions, settings and default reporting are the same for every board and are described in the [main README](../README.md).

## Build

Zigbee only:

```
make -s -j VERSION_BIN=_v0140 PROJECT_NAME=LKTMZL02Z POJECT_DEF="-DBOARD=BOARD_LKTMZL02" ZNAME="Tuya:LKTMZL02-z"
```

Zigbee with BLE OTA (see [ZBdevices.md](../ZBdevices.md)):

```
make -s -j VERSION_BIN=_zb0018 PROJECT_NAME=LKTMZL02Z USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_LKTMZL02" ZNAME="Tuya:LKTMZL02-z"
```

## Prebuilt firmware

- Zigbee only, for a hardware programmer: [bin/LKTMZL02Z_v0140.bin](../bin/LKTMZL02Z_v0140.bin)
- Zigbee only, OTA image: [bin/1141-021f-01403001-LKTMZL02Z.zigbee](../bin/1141-021f-01403001-LKTMZL02Z.zigbee)
- Zigbee with BLE OTA, for a hardware programmer: [bin_zb/LKTMZL02Z_zb0018.bin](../bin_zb/LKTMZL02Z_zb0018.bin)
- Zigbee with BLE OTA, OTA image: [bin_zb/1141-021f-00183001-LKTMZL02Z.zigbee](../bin_zb/1141-021f-00183001-LKTMZL02Z.zigbee)

[bin/README.md](../bin/README.md) explains the file names, including the images that convert a device from its stock firmware.
