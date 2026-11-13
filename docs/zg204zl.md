# Sonoff ZG-204ZL

Battery Zigbee motion (PIR) and illuminance sensor.

Upstream description: HOBEIAN ZG-204ZL PIR + LUX sensor

> This page was generated from the board header and the build scripts. It has
> not been checked against the hardware.

## Summary

| | |
|---|---|
| Joins Zigbee as | `Sonoff` / `ZG-204ZL-z` |
| Build name | `ZG204ZL` |
| Board define | `BOARD_ZG204ZL` (id 50) |
| Board header | [src/board_zg204zl.h](../src/board_zg204zl.h) |
| Chip (header comment) | TLSR8253 |
| Battery | CR2450 |
| Display | none |
| Motion sensor | PIR |
| Illuminance sensor | yes, read through the ADC |
| Hardware notes (upstream) | <https://pvvx.github.io/ZG-204ZL-3.0>, <https://pvvx.github.io/ZG-204ZL-3.0/> |

## Pins

Pin assignments the firmware uses for this build:

| Function | Pin |
|---|---|
| Button | PA1 |
| LED | PD7 |
| Motion / presence input | PD4 |
| Illuminance ADC | PC4 |

Notes from the board header. Where they disagree with the table above, the table is what the firmware uses:

```
 https://pvvx.github.io/ZG-204ZL-3.0/

TLSR8253

GPIO_PA1 - KEY
GPIO_PA7 - SWS, (debug TX)
GPIO_PB1 - TX
GPIO_PB7 - RX
GPIO_PC4 - illuminance Sensor
GPIO_PD3 - illuminance Sensor Power
GPIO_PD4 - PIR
GPIO_PD7 - LED to +Vbat
```

## Zigbee clusters

Set by `DEV_SERVICES` in the board header and [src/app_cfg.h](../src/app_cfg.h):

- Basic (`0x0000`), Power Configuration (`0x0001`), Poll Control (`0x0020`), OTA Upgrade (`0x0019`)
- Illuminance Measurement (`0x0400`), Illuminance Level Sensing (`0x0401`)
- Occupancy Sensing (`0x0406`)
- On/Off (`0x0006`) as an output cluster, enabled by `USE_REMOTE_ONOFF`

Button actions, settings and default reporting are the same for every board and are described in the [main README](../README.md).

## Build

Zigbee only:

```
make -s -j VERSION_BIN=_v0140 PROJECT_NAME=ZG204ZL POJECT_DEF="-DBOARD=BOARD_ZG204ZL" ZNAME="Sonoff:ZG-204ZL-z"
```

There is no Zigbee with BLE OTA build of this board in `WinMakeZB.cmd`.

## Prebuilt firmware

- Zigbee only, for a hardware programmer: [bin/ZG204ZL_v0140.bin](../bin/ZG204ZL_v0140.bin)
- Zigbee only, OTA image: [bin/1141-0232-01403001-ZG204ZL.zigbee](../bin/1141-0232-01403001-ZG204ZL.zigbee)

[bin/README.md](../bin/README.md) explains the file names, including the images that convert a device from its stock firmware.
