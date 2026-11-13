# Sonoff ZG-204ZV

Battery Zigbee temperature and humidity, presence (XBR818 radar) and illuminance sensor.

Upstream description: HOBEIAN-ZG-204ZV T&H + LUX + Radar Sensor XBR818

> This page was generated from the board header and the build scripts. It has
> not been checked against the hardware.

## Summary

| | |
|---|---|
| Joins Zigbee as | `Sonoff` / `ZG-204ZV-z` |
| Build name | `ZG204ZV` |
| Board define | `BOARD_ZG204ZV` (id 51) |
| Board header | [src/board_zg204zv.h](../src/board_zg204zv.h) |
| Chip (header comment) | TLSR8253F512ET32 512K Flash |
| Battery | 2 x AAA |
| Display | none |
| Temperature / humidity sensor | AHT20/AHT30 |
| Presence sensor | XBR818 radar |
| Illuminance sensor | yes, read through the ADC |
| Hardware notes (upstream) | <https://pvvx.github.io/ZG-204ZL-3.0/> |

## Pins

Pin assignments the firmware uses for this build:

| Function | Pin |
|---|---|
| Button | PD4 |
| LED | PC2 |
| I2C (software) | SCL PC4, SDA PC3 |
| Motion / presence input | PD7 |
| XBR818 radar | SCL PA0, SDA PA1, OUT PD7 |
| Illuminance ADC | PB5 |

Notes from the board header (the header is shared by several builds, so the notes cover all of them). Where they disagree with the table above, the table is what the firmware uses:

```
 https://pvvx.github.io/ZG-204ZL-3.0/
 TLSR8253F512ET32 512K Flash
 GPIO_PA7 - SWS, free, (debug TX)
 GPIO_PB1 - TX
 GPIO_PC2 - LED (+Vbat)
 GPIO_PB5 - R lx
 GPIO_PB6 - R lx
 GPIO_PB7 - RX
 GPIO_PD4 - KEY (GND)
 GPIO_PC3 - SDA
 GPIO_PC4 - SCL
 ver1:
 GPIO_PD7 - XBR818 OUT
 GPIO_PA0 - XBR818 SCL
 GPIO_PA1 - XBR818 SDA
 ver2:
 GPIO_PD7 - XBR818 SCL
 GPIO_PA0 - XBR818 SDA
 GPIO_PA1 - XBR818 OUT
```

## Zigbee clusters

Set by `DEV_SERVICES` in the board header and [src/app_cfg.h](../src/app_cfg.h):

- Basic (`0x0000`), Power Configuration (`0x0001`), Poll Control (`0x0020`), OTA Upgrade (`0x0019`)
- Temperature Measurement (`0x0402`), Relative Humidity Measurement (`0x0405`), Thermostat User Interface Configuration (`0x0204`)
- Illuminance Measurement (`0x0400`), Illuminance Level Sensing (`0x0401`)
- Occupancy Sensing (`0x0406`)
- On/Off (`0x0006`) as an output cluster, enabled by `USE_REMOTE_ONOFF`

Button actions, settings and default reporting are the same for every board and are described in the [main README](../README.md).

## Build

Zigbee only:

```
make -s -j VERSION_BIN=_v0140 PROJECT_NAME=ZG204ZV POJECT_DEF="-DBOARD=BOARD_ZG204ZV" ZNAME="Sonoff:ZG-204ZV-z"
```

There is no Zigbee with BLE OTA build of this board in `WinMakeZB.cmd`.

## Prebuilt firmware

- Zigbee only, for a hardware programmer: [bin/ZG204ZV_v0140.bin](../bin/ZG204ZV_v0140.bin)
- Zigbee only, OTA image: [bin/1141-0233-01403001-ZG204ZV.zigbee](../bin/1141-0233-01403001-ZG204ZV.zigbee)

[bin/README.md](../bin/README.md) explains the file names, including the images that convert a device from its stock firmware.
