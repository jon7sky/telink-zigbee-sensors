# ZBeacon TH01-2 (ZB2TH01SHTC3 build)

Battery Zigbee temperature and humidity sensor.

Upstream description: ZBEACON-TH01-v2 PCB variant: SHTC3, SDA/SCL swapped, no TX pad (TS0201_TZ3000_rdhukkmi)

> This page was generated from the board header and the build scripts. It has
> not been checked against the hardware.

## Summary

| | |
|---|---|
| Joins Zigbee as | `ZBeacon` / `TH01-2-z` |
| Build name | `ZB2TH01SHTC3` |
| Board define | `BOARD_ZBEACON2_TH01_SHTC3` (id 58) |
| Board header | [src/board_zbeacon2_th01_shtc3.h](../src/board_zbeacon2_th01_shtc3.h) |
| Chip (header comment) | TLSR8656F512ET32 |
| Battery | 2 x AAA |
| Display | none |
| Temperature / humidity sensor | SHTC3 |
| Hardware notes (upstream) | <https://github.com/pvvx/ZigbeeTLc/issues/211> |

## Pins

Pin assignments the firmware uses for this build:

| Function | Pin |
|---|---|
| Button | PB7 |
| LED | PB6 |
| I2C (software) | SCL PA0, SDA PD4 |

Notes from the board header. Where they disagree with the table above, the table is what the firmware uses:

```
 TLSR8656F512ET32
ZBEACON-TH01 v2.0 PCB variant, also sold as TS0201_TZ3000_rdhukkmi
https://github.com/pvvx/ZigbeeTLc/issues/211
Differences from BOARD_ZBEACON2_TH01: SHTC3 sensor, SDA/SCL swapped, no TX pad
GPIO_A0 - SCL
GPIO_A7 - SWS
GPIO_B6 - LED
GPIO_B7 - KEY
GPIO_D4 - SDA
```

## Zigbee clusters

Set by `DEV_SERVICES` in the board header and [src/app_cfg.h](../src/app_cfg.h):

- Basic (`0x0000`), Power Configuration (`0x0001`), Poll Control (`0x0020`), OTA Upgrade (`0x0019`)
- Temperature Measurement (`0x0402`), Relative Humidity Measurement (`0x0405`), Thermostat User Interface Configuration (`0x0204`)

Button actions, settings and default reporting are the same for every board and are described in the [main README](../README.md).

## Build

Zigbee only:

```
make -s -j VERSION_BIN=_v0140 PROJECT_NAME=ZB2TH01SHTC3 POJECT_DEF="-DBOARD=BOARD_ZBEACON2_TH01_SHTC3" ZNAME="ZBeacon:TH01-2-z"
```

Zigbee with BLE OTA (see [ZBdevices.md](../ZBdevices.md)):

```
make -s -j VERSION_BIN=_zb0018 PROJECT_NAME=ZB2TH01SHTC3 USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_ZBEACON2_TH01_SHTC3" ZNAME="ZBeacon:TH01-2-z"
```

## Prebuilt firmware

- Zigbee only, for a hardware programmer: [bin/ZB2TH01SHTC3_v0140.bin](../bin/ZB2TH01SHTC3_v0140.bin)
- Zigbee only, OTA image: [bin/1141-023a-01403001-ZB2TH01SHTC3.zigbee](../bin/1141-023a-01403001-ZB2TH01SHTC3.zigbee)
- Zigbee with BLE OTA, for a hardware programmer: [bin_zb/ZB2TH01SHTC3_zb0018.bin](../bin_zb/ZB2TH01SHTC3_zb0018.bin)
- Zigbee with BLE OTA, OTA image: [bin_zb/1141-023a-00183001-ZB2TH01SHTC3.zigbee](../bin_zb/1141-023a-00183001-ZB2TH01SHTC3.zigbee)

[bin/README.md](../bin/README.md) explains the file names, including the images that convert a device from its stock firmware.
