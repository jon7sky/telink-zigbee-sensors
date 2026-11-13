# ZBeacon MC

Battery Zigbee temperature and humidity sensor.

Upstream description: ZigBee-MC, 2xAAA, CHT8305

> This page was generated from the board header and the build scripts. It has
> not been checked against the hardware.

## Summary

| | |
|---|---|
| Joins Zigbee as | `ZBeacon` / `MC-z` |
| Build name | `ZB_MC` |
| Board define | `BOARD_ZB_MC` (id 46) |
| Board header | [src/board_zb_mc.h](../src/board_zb_mc.h) |
| Chip (header comment) | ZTU Tuya module |
| Battery | 2 x AAA |
| Display | none |
| Temperature / humidity sensor | CHT8305, CHT8215, AHT20/AHT30, SHT4x, SHT30 (drivers enabled in this build) |
| Hardware notes (upstream) | <https://pvvx.github.io/Zbeacon-MC> |

## Pins

Pin assignments the firmware uses for this build:

| Function | Pin |
|---|---|
| Button | PD3 |
| LED | PB4 |
| I2C (software) | SCL PC0, SDA PD7 |

Notes from the board header. Where they disagree with the table above, the table is what the firmware uses:

```
ZTU Tuya module
GPIO_A7 - SWS
GPIO_B1 - TX
GPIO_B4 - LED, On: "1"
GPIO_C0 - SCL
GPIO_C3 - ALERT
GPIO_B7 - RX
GPIO_D3 - KEY, On: "0"
GPIO_D7 - SDA
```

## Zigbee clusters

Set by `DEV_SERVICES` in the board header and [src/app_cfg.h](../src/app_cfg.h):

- Basic (`0x0000`), Power Configuration (`0x0001`), Poll Control (`0x0020`), OTA Upgrade (`0x0019`)
- Temperature Measurement (`0x0402`), Relative Humidity Measurement (`0x0405`), Thermostat User Interface Configuration (`0x0204`)

Button actions, settings and default reporting are the same for every board and are described in the [main README](../README.md).

## Build

Zigbee only:

```
make -s -j VERSION_BIN=_v0140 PROJECT_NAME=ZB_MC POJECT_DEF="-DBOARD=BOARD_ZB_MC" ZNAME="ZBeacon:MC-z"
```

Zigbee with BLE OTA (see [ZBdevices.md](../ZBdevices.md)):

```
make -s -j VERSION_BIN=_zb0018 PROJECT_NAME=ZB_MC USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_ZB_MC" ZNAME="ZBeacon:MC-z"
```

## Prebuilt firmware

- Zigbee only, for a hardware programmer: [bin/ZB_MC_v0140.bin](../bin/ZB_MC_v0140.bin)
- Zigbee only, OTA image: [bin/1141-022e-01403001-ZB_MC.zigbee](../bin/1141-022e-01403001-ZB_MC.zigbee)
- Zigbee with BLE OTA, for a hardware programmer: [bin_zb/ZB_MC_zb0018.bin](../bin_zb/ZB_MC_zb0018.bin)
- Zigbee with BLE OTA, OTA image: [bin_zb/1141-022e-00183001-ZB_MC.zigbee](../bin_zb/1141-022e-00183001-ZB_MC.zigbee)

[bin/README.md](../bin/README.md) explains the file names, including the images that convert a device from its stock firmware.
