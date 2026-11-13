# Tuya TH05

Battery Zigbee temperature and humidity sensor with an LCD.

Upstream description: Tuya ZigBee TS0601_TZE204

> This page was generated from the board header and the build scripts. It has
> not been checked against the hardware.

## Summary

| | |
|---|---|
| Joins Zigbee as | `Tuya` / `TH05-z` |
| Build name | `ZTH05` |
| Board define | `BOARD_ZTH05` (id 33) |
| Board header | [src/board_zth05.h](../src/board_zth05.h) |
| Chip (header comment) | TLSR8258 |
| Battery | CR2032 |
| Display | LCD, driver [src/lcd_zth05.c](../src/lcd_zth05.c) |
| Temperature / humidity sensor | CHT8305, CHT8215, AHT20/AHT30, SHT4x, SHTC3, SHT30 (drivers enabled in this build) |
| Hardware notes (upstream) | <https://pvvx.github.io/TS0601_TZE204>, <https://pvvx.github.io/TS0601_TZE204/> |

## Pins

Pin assignments the firmware uses for this build:

| Function | Pin |
|---|---|
| Button | PB4 |
| I2C (hardware) | PC2 and PC3 |

Notes from the board header. Where they disagree with the table above, the table is what the firmware uses:

```
 https://pvvx.github.io/TS0601_TZE204/

TLSR8258

 GPIO_PA0 - free (Reed Switch, input)
 GPIO_PA1 - free
 GPIO_PA7 - SWS, (debug TX)
 GPIO_PB1 - free
 GPIO_PB4 - KEY
 GPIO_PB5 - free, (TRG)
 GPIO_PB6 - free
 GPIO_PB7 - free
 GPIO_PC0 - free
 GPIO_PC1 - free
 GPIO_PC2 - SDA
 GPIO_PC3 - SCL
 GPIO_PC4 - free
 GPIO_PD2 - free, (RDS)
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
make -s -j VERSION_BIN=_v0140 PROJECT_NAME=ZTH05 POJECT_DEF="-DBOARD=BOARD_ZTH05" ZNAME="Tuya:TH05-z"
```

Zigbee with BLE OTA (see [ZBdevices.md](../ZBdevices.md)):

```
make -s -j VERSION_BIN=_zb0018 PROJECT_NAME=ZTH05 USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_ZTH05" ZNAME="Tuya:TH05-z"
```

## Prebuilt firmware

- Zigbee only, for a hardware programmer: [bin/ZTH05_v0140.bin](../bin/ZTH05_v0140.bin)
- Zigbee only, OTA image: [bin/1141-0221-01403001-ZTH05.zigbee](../bin/1141-0221-01403001-ZTH05.zigbee)
- Zigbee with BLE OTA, for a hardware programmer: [bin_zb/ZTH05_zb0018.bin](../bin_zb/ZTH05_zb0018.bin)
- Zigbee with BLE OTA, OTA image: [bin_zb/1141-0221-00183001-ZTH05.zigbee](../bin_zb/1141-0221-00183001-ZTH05.zigbee)

[bin/README.md](../bin/README.md) explains the file names, including the images that convert a device from its stock firmware.
