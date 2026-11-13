# Tuya ZTH01

Battery Zigbee temperature and humidity sensor.

Upstream description: ZigBee ZTH01

> This page was generated from the board header and the build scripts. It has
> not been checked against the hardware.

## Summary

| | |
|---|---|
| Joins Zigbee as | `Tuya` / `ZTH01-z` |
| Build name | `ZTH01Z` |
| Board define | `BOARD_ZTH01` (id 27) |
| Board header | [src/board_zth01_02.h](../src/board_zth01_02.h) |
| Chip (header comment) | TLSR825x 1M Flash |
| Battery | 2 x AAA |
| Display | none |
| Temperature / humidity sensor | CHT8305, CHT8215, AHT20/AHT30, SHT4x (drivers enabled in this build) |
| Hardware notes (upstream) | <https://pvvx.github.io/TS0601_TZE200_zth01> |

## Pins

Pin assignments the firmware uses for this build:

| Function | Pin |
|---|---|
| Button | PB4 |
| LED | PB5 |
| I2C (hardware) | PC2 and PC3 |

Notes from the board header (the header is shared by several builds, so the notes cover all of them). Where they disagree with the table above, the table is what the firmware uses:

```
 https://pvvx.github.io/TS0601_TZE200_zth01

TLSR825x 1M Flash

GPIO_PB1 - TX
GPIO_PB4 - KEY
GPIO_PB5 - LED
GPIO_PB7 - RX
GPIO_PC2 - SDA
GPIO_PC3 - SCL
```

## Zigbee clusters

Set by `DEV_SERVICES` in the board header and [src/app_cfg.h](../src/app_cfg.h):

- Basic (`0x0000`), Power Configuration (`0x0001`), Poll Control (`0x0020`), OTA Upgrade (`0x0019`)
- Temperature Measurement (`0x0402`), Relative Humidity Measurement (`0x0405`), Thermostat User Interface Configuration (`0x0204`)

Button actions, settings and default reporting are the same for every board and are described in the [main README](../README.md).

## Build

Zigbee only:

```
make -s -j VERSION_BIN=_v0140 PROJECT_NAME=ZTH01Z POJECT_DEF="-DBOARD=BOARD_ZTH01" ZNAME="Tuya:ZTH01-z"
```

Zigbee with BLE OTA (see [ZBdevices.md](../ZBdevices.md)):

```
make -s -j VERSION_BIN=_zb0018 PROJECT_NAME=ZTH01Z USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_ZTH01" ZNAME="Tuya:ZTH01-z"
```

## Prebuilt firmware

- Zigbee only, for a hardware programmer: [bin/ZTH01Z_v0140.bin](../bin/ZTH01Z_v0140.bin)
- Zigbee only, OTA image: [bin/1141-021b-01403001-ZTH01Z.zigbee](../bin/1141-021b-01403001-ZTH01Z.zigbee)
- Zigbee with BLE OTA, for a hardware programmer: [bin_zb/ZTH01Z_zb0018.bin](../bin_zb/ZTH01Z_zb0018.bin)
- Zigbee with BLE OTA, OTA image: [bin_zb/1141-021b-00183001-ZTH01Z.zigbee](../bin_zb/1141-021b-00183001-ZTH01Z.zigbee)

[bin/README.md](../bin/README.md) explains the file names, including the images that convert a device from its stock firmware.
