# ZBeacon TH01-2 (ZB2TH01 build)

Battery Zigbee temperature and humidity sensor.

Upstream description: ZBEACON-TH01-v2, 2xAAA , SHT4X/G40

> This page was generated from the board header and the build scripts. It has
> not been checked against the hardware.

## Summary

| | |
|---|---|
| Joins Zigbee as | `ZBeacon` / `TH01-2-z` |
| Build name | `ZB2TH01` |
| Board define | `BOARD_ZBEACON2_TH01` (id 47) |
| Board header | [src/board_zbeacon2_th01.h](../src/board_zbeacon2_th01.h) |
| Chip (header comment) | TLSR8250F512ET32 512K Flash (BT3L Tuya module) |
| Battery | 2 x AAA |
| Display | none |
| Temperature / humidity sensor | CHT8305, CHT8215, AHT20/AHT30, SHT4x, SHT30 (drivers enabled in this build) |
| Hardware notes (upstream) | <https://pvvx.github.io/Zbeacon-TH01-v2.0>, <https://pvvx.github.io/Zbeacon-TH01/> |

## Pins

Pin assignments the firmware uses for this build:

| Function | Pin |
|---|---|
| Button | PB7 |
| LED | PB6 |
| I2C (software) | SCL PD4, SDA PA0 |

Notes from the board header. Where they disagree with the table above, the table is what the firmware uses:

```
 https://pvvx.github.io/Zbeacon-TH01/
TLSR8250F512ET32 512K Flash (BT3L Tuya module)
GPIO_A0 - SDA
GPIO_A7 - SWS
GPIO_B1 - TX
GPIO_B4 - SCL
GPIO_B5 - KEY
GPIO_B7 - RX
GPIO_C3 - LED
```

## Zigbee clusters

Set by `DEV_SERVICES` in the board header and [src/app_cfg.h](../src/app_cfg.h):

- Basic (`0x0000`), Power Configuration (`0x0001`), Poll Control (`0x0020`), OTA Upgrade (`0x0019`)
- Temperature Measurement (`0x0402`), Relative Humidity Measurement (`0x0405`), Thermostat User Interface Configuration (`0x0204`)

Button actions, settings and default reporting are the same for every board and are described in the [main README](../README.md).

## Build

Zigbee only:

```
make -s -j VERSION_BIN=_v0140 PROJECT_NAME=ZB2TH01 POJECT_DEF="-DBOARD=BOARD_ZBEACON2_TH01" ZNAME="ZBeacon:TH01-2-z"
```

There is no Zigbee with BLE OTA build of this board in `WinMakeZB.cmd`.

## Prebuilt firmware

- Zigbee only, for a hardware programmer: [bin/ZB2TH01_v0140.bin](../bin/ZB2TH01_v0140.bin)
- Zigbee only, OTA image: [bin/1141-022f-01403001-ZB2TH01.zigbee](../bin/1141-022f-01403001-ZB2TH01.zigbee)

[bin/README.md](../bin/README.md) explains the file names, including the images that convert a device from its stock firmware.
