# Sonoff TH03

Battery Zigbee temperature and humidity sensor with an LCD.

Upstream description: ZigBee TH03 TS0201(_TZ3000_gohcgzj7, _TZ3000_bguser20) LCD

> This page was generated from the board header and the build scripts. It has
> not been checked against the hardware.

## Summary

| | |
|---|---|
| Joins Zigbee as | `Sonoff` / `TH03-z` |
| Build name | `TH03Z` |
| Board define | `BOARD_ZTH03` (id 30) |
| Board header | [src/board_zth03.h](../src/board_zth03.h) |
| Chip (header comment) | TLSR8258 |
| Battery | CR2032 |
| Display | LCD, driver [src/lcd_zth03.c](../src/lcd_zth03.c) |
| Temperature / humidity sensor | CHT8305, CHT8215, AHT20/AHT30, SHT4x, SHTC3, SHT30 (drivers enabled in this build) |
| Hardware notes (upstream) | <https://pvvx.github.io/TS0201_TZ3000_TH03>, <https://pvvx.github.io/TS0201_TZ3000_TH03/> |

## Pins

Pin assignments the firmware uses for this build:

| Function | Pin |
|---|---|
| Button | PD4 |
| LED | PC0 |
| I2C (hardware) | PC2 and PC3 |
| I2C for the display | SCL PB1, SDA PB7 |

Notes from the board header. Where they disagree with the table above, the table is what the firmware uses:

```
 https://pvvx.github.io/TS0201_TZ3000_TH03/

TLSR8258

GPIO_PA0 - free (Reed Switch, input)
GPIO_PA1 - free
GPIO_PA7 - SWS, (debug TX)
GPIO_PB1 - SCL, used I2C LCD
GPIO_PB4 - free, (TRG)
GPIO_PB5 - free
GPIO_PB6 - free
GPIO_PB7 - SDA, used I2C LCD
GPIO_PC0 - LED
GPIO_PC1 - free
GPIO_PC2 - SDA, used I2C Sensor
GPIO_PC3 - SCL, used I2C Sensor
GPIO_PC4 - free
GPIO_PD2 - free
GPIO_PD3 - free
GPIO_PD4 - used KEY
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
make -s -j VERSION_BIN=_v0140 PROJECT_NAME=TH03Z POJECT_DEF="-DBOARD=BOARD_ZTH03" ZNAME="Sonoff:TH03-z"
```

Zigbee with BLE OTA (see [ZBdevices.md](../ZBdevices.md)):

```
make -s -j VERSION_BIN=_zb0018 PROJECT_NAME=TH03Z USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_ZTH03" ZNAME="Sonoff:TH03-z"
```

## Prebuilt firmware

- Zigbee only, for a hardware programmer: [bin/TH03Z_v0140.bin](../bin/TH03Z_v0140.bin)
- Zigbee only, OTA image: [bin/1141-021e-01403001-TH03Z.zigbee](../bin/1141-021e-01403001-TH03Z.zigbee)
- Zigbee with BLE OTA, for a hardware programmer: [bin_zb/TH03Z_zb0018.bin](../bin_zb/TH03Z_zb0018.bin)
- Zigbee with BLE OTA, OTA image: [bin_zb/1141-021e-00183001-TH03Z.zigbee](../bin_zb/1141-021e-00183001-TH03Z.zigbee)

[bin/README.md](../bin/README.md) explains the file names, including the images that convert a device from its stock firmware.
