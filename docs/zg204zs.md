# Sonoff ZG-204ZS

Battery Zigbee presence (PIR and RC2417 mmWave radar) and illuminance sensor.

Upstream description: ZigBee only, HOBEIAN/Tuya ZG-204ZS / ZG-204ZM PIR + mmWave Radar RC2417 + LUX

> This page was generated from the board header and the build scripts. It has
> not been checked against the hardware.

## Summary

| | |
|---|---|
| Joins Zigbee as | `Sonoff` / `ZG-204ZS-z` |
| Build name | `ZG204ZS` |
| Board define | `BOARD_ZG204ZS` (id 60) |
| Board header | [src/board_zg204zs.h](../src/board_zg204zs.h) |
| Chip (header comment) | TLSR8258F512ET32 |
| Battery | 2 x AAA |
| Display | none |
| Presence sensor | PIR and RC2417 mmWave radar |
| Illuminance sensor | yes, read through the ADC |

## Pins

Pin assignments the firmware uses for this build:

| Function | Pin |
|---|---|
| Button | PA1 |
| LED | PD7 |
| Motion / presence input | PD4 |
| RC2417 radar | TX PB1, RX PB7, power PB5, OUT PB6 |
| Illuminance ADC | PC4 |

Notes from the board header. Where they disagree with the table above, the table is what the firmware uses:

```
 Pin assignments for ZG-204ZS V5.2 / ZG-204ZM:
   TLSR8258F512ET32
   GPIO_PA1 - KEY / Button 1 (Active LOW, 10k pullup)
   GPIO_PA7 - SWS (debug TX / flash)
   GPIO_PB1 - Radar UART TX (MCU TX -> Radar R pin, 115200 8N1)
   GPIO_PB5 - Radar Power Gate (A19T P-MOSFET gate, Active LOW: 0 = Radar ON, 1 = Radar OFF)
   GPIO_PB6 - Radar OUT (Digital presence detection input, Active HIGH, 100k pulldown)
   GPIO_PB7 - Radar UART RX (MCU RX <- Radar T pin, 115200 8N1)
   GPIO_PC1 - Illuminance Sensor Power switch (Active HIGH)
   GPIO_PC4 - Illuminance Sensor ADC input (C4P)
   GPIO_PD4 - PIR sensor motion input (Active HIGH)
   GPIO_PD7 - LED to +Vbat (Active LOW)
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
make -s -j VERSION_BIN=_v0140 PROJECT_NAME=ZG204ZS POJECT_DEF="-DBOARD=BOARD_ZG204ZS" ZNAME="Sonoff:ZG-204ZS-z"
```

There is no Zigbee with BLE OTA build of this board in `WinMakeZB.cmd`.

## Prebuilt firmware

- Zigbee only, for a hardware programmer: [bin/ZG204ZS_v0140.bin](../bin/ZG204ZS_v0140.bin)
- Zigbee only, OTA image: [bin/1141-023c-01403001-ZG204ZS.zigbee](../bin/1141-023c-01403001-ZG204ZS.zigbee)

[bin/README.md](../bin/README.md) explains the file names, including the images that convert a device from its stock firmware.
