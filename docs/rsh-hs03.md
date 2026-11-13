# Tuya TY0201

Battery Zigbee temperature and humidity sensor with an LCD.

Upstream description: Tuya ZigBee Temperature/Humidity Sensor "TY0201_TZ3000_bjawzodf", PCB designator "RSH-HS03-V2.0-241018"

> This page was generated from the board header and the build scripts. It has
> not been checked against the hardware.

## Summary

| | |
|---|---|
| Joins Zigbee as | `Tuya` / `TY0201-z` |
| Build name | `ZTY0201` |
| Board define | `BOARD_RSH_HS03` (id 48) |
| Board header | [src/board_rsh_hs03.h](../src/board_rsh_hs03.h) |
| Chip (header comment) | TLSR8258 |
| Battery | 2 x AAA |
| Display | LCD, driver [src/lcd_rsh_hs03.c](../src/lcd_rsh_hs03.c) |
| Temperature / humidity sensor | CHT8215, AHT20/AHT30 (drivers enabled in this build) |
| Hardware notes (upstream) | <https://pvvx.github.io/TY0201_TZ3000_bjawzodf>, <https://github.com/pvvx/ZigbeeTLc/issues/107#issuecomment-2227385821> |

## Pins

Pin assignments the firmware uses for this build:

| Function | Pin |
|---|---|
| Button | PB4 |
| I2C (software) | SCL PA0, SDA PA1 |

Notes from the board header. Where they disagree with the table above, the table is what the firmware uses:

```
  This file is for Zigbee T&H Sensors with "RSH-HS03-V2.0" designator on PCB
  Zigbee model/manufacturer TY0201_TZ3000_bjawzodf

  There are 2 HW version of this sensor: with MRT08S SO-8 chip soldered on PCB
  and version without this chip but with 2 zero ohm resistors near PA0, PA1.

  HARDWARE MODIFICATION IS REQUIRED TO RUN THIS FIRMWARE ON BOARDS WITH MRT08S,
  see https://github.com/pvvx/ZigbeeTLc/issues/107#issuecomment-2227385821

TLSR8258

GPIO_PA0 - free or soft I2C SCL on boards without MRT08S chip
GPIO_PA1 - free or soft I2C SDA on boards without MRT08S chip
GPIO_PA7 - SWS
GPIO_PB4 - KEY
GPIO_PC2 - UART TX on boards with MRT08S chip, debug header TX, used for HW I2C SDA after MRT08S removal
GPIO_PC3 - UART RX on boards with MRT08S chip, debug header RX, used for HW I2C SCL after MRT08S removal
```

## Zigbee clusters

Set by `DEV_SERVICES` in the board header and [src/app_cfg.h](../src/app_cfg.h):

- Basic (`0x0000`), Power Configuration (`0x0001`), Poll Control (`0x0020`), OTA Upgrade (`0x0019`)
- Temperature Measurement (`0x0402`), Relative Humidity Measurement (`0x0405`), Thermostat User Interface Configuration (`0x0204`)

Button actions, settings and default reporting are the same for every board and are described in the [main README](../README.md).

## Build

Zigbee only:

```
make -s -j VERSION_BIN=_v0140 PROJECT_NAME=ZTY0201 POJECT_DEF="-DBOARD=BOARD_RSH_HS03" ZNAME="Tuya:TY0201-z"
```

Zigbee with BLE OTA (see [ZBdevices.md](../ZBdevices.md)):

```
make -s -j VERSION_BIN=_zb0018 PROJECT_NAME=ZTY0201 USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_RSH_HS03" ZNAME="Tuya:TY0201-z"
```

## Prebuilt firmware

- Zigbee only, for a hardware programmer: [bin/ZTY0201_v0140.bin](../bin/ZTY0201_v0140.bin)
- Zigbee only, OTA image: [bin/1141-0230-01403001-ZTY0201.zigbee](../bin/1141-0230-01403001-ZTY0201.zigbee)
- Zigbee with BLE OTA, for a hardware programmer: [bin_zb/ZTY0201_zb0018.bin](../bin_zb/ZTY0201_zb0018.bin)
- Zigbee with BLE OTA, OTA image: [bin_zb/1141-0230-00183001-ZTY0201.zigbee](../bin_zb/1141-0230-00183001-ZTY0201.zigbee)

[bin/README.md](../bin/README.md) explains the file names, including the images that convert a device from its stock firmware.
