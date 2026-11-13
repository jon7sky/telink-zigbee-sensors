# Tuya TH03Z

Battery Zigbee temperature and humidity sensor.

Upstream description: ZigBee TH03Z

> This page was generated from the board header and the build scripts. It has
> not been checked against the hardware.

## Summary

| | |
|---|---|
| Joins Zigbee as | `Tuya` / `TH03Z-z` |
| Build name | `ZTH03Z` |
| Board define | `BOARD_TH03Z` (id 22) |
| Board header | [src/board_th03z.h](../src/board_th03z.h) |
| Chip (header comment) | TLSR825x 1M Flash |
| Battery | CR2450 |
| Display | none |
| Temperature / humidity sensor | CHT8305, CHT8215, SHT30 (drivers enabled in this build) |
| Hardware notes (upstream) | <https://pvvx.github.io/TH03Z>, <https://pvvx.github.io/TH03Z/> |

## Pins

Pin assignments the firmware uses for this build:

| Function | Pin |
|---|---|
| Button | PC0 |
| LED | PB4 |
| I2C (hardware) | PC2 and PC3 |

Notes from the board header. Where they disagree with the table above, the table is what the firmware uses:

```
 https://pvvx.github.io/TH03Z/

TLSR825x 1M Flash

GPIO_PA7 - SWS, free, (debug TX)
GPIO_PB1 - TX
GPIO_PB4 - LED
GPIO_PB7 - RX
GPIO_PC0 - KEY
GPIO_PC2 - SDA
GPIO_PC3 - SCL
GPIO_PD7 - Alert
```

## Zigbee clusters

Set by `DEV_SERVICES` in the board header and [src/app_cfg.h](../src/app_cfg.h):

- Basic (`0x0000`), Power Configuration (`0x0001`), Poll Control (`0x0020`), OTA Upgrade (`0x0019`)
- Temperature Measurement (`0x0402`), Relative Humidity Measurement (`0x0405`), Thermostat User Interface Configuration (`0x0204`)

Button actions, settings and default reporting are the same for every board and are described in the [main README](../README.md).

## Build

Zigbee only:

```
make -s -j VERSION_BIN=_v0140 PROJECT_NAME=ZTH03Z POJECT_DEF="-DBOARD=BOARD_TH03Z" ZNAME="Tuya:TH03Z-z"
```

Zigbee with BLE OTA (see [ZBdevices.md](../ZBdevices.md)):

```
make -s -j VERSION_BIN=_zb0018 PROJECT_NAME=ZTH03Z USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_TH03Z" ZNAME="Tuya:TH03Z-z"
```

## Prebuilt firmware

- Zigbee only, for a hardware programmer: [bin/ZTH03Z_v0140.bin](../bin/ZTH03Z_v0140.bin)
- Zigbee only, OTA image: [bin/1141-0216-01403001-ZTH03Z.zigbee](../bin/1141-0216-01403001-ZTH03Z.zigbee)
- Zigbee with BLE OTA, for a hardware programmer: [bin_zb/ZTH03Z_zb0018.bin](../bin_zb/ZTH03Z_zb0018.bin)
- Zigbee with BLE OTA, OTA image: [bin_zb/1141-0216-00183001-ZTH03Z.zigbee](../bin_zb/1141-0216-00183001-ZTH03Z.zigbee)

[bin/README.md](../bin/README.md) explains the file names, including the images that convert a device from its stock firmware.
