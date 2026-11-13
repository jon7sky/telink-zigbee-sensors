# Sonoff ZG-204ZV2-TH

Battery Zigbee temperature and humidity, presence (XBR818 radar) and illuminance sensor.

Upstream description: ZigBee only, HOBEIAN-ZG-204ZV v2 T&H + LUX + Radar Sensor XBR818 (HOBEIAN GPIO)

> This page was generated from the board header and the build scripts. It has
> not been checked against the hardware.

## Summary

| | |
|---|---|
| Joins Zigbee as | `Sonoff` / `ZG-204ZV2-TH-z` |
| Build name | `ZG204ZV2-TH` |
| Board define | `BOARD_ZG204ZV2_TH` (id 56) |
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
| Motion / presence input | PA1 |
| XBR818 radar | SCL PD7, SDA PA0, OUT PA1 |
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
make -s -j VERSION_BIN=_v0140 PROJECT_NAME=ZG204ZV2-TH POJECT_DEF="-DBOARD=BOARD_ZG204ZV2_TH" ZNAME="Sonoff:ZG-204ZV2-TH-z"
```

Zigbee with BLE OTA (see [ZBdevices.md](../ZBdevices.md)):

```
make -s -j VERSION_BIN=_zb0018 PROJECT_NAME=ZG204ZV2-TH USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_ZG204ZV2_TH" ZNAME="Sonoff:ZG-204ZV2-TH-z"
```

## Prebuilt firmware

- Zigbee only, for a hardware programmer: [bin/ZG204ZV2-TH_v0140.bin](../bin/ZG204ZV2-TH_v0140.bin)
- Zigbee only, OTA image: [bin/1141-0238-01403001-ZG204ZV2-TH.zigbee](../bin/1141-0238-01403001-ZG204ZV2-TH.zigbee)
- Zigbee with BLE OTA, for a hardware programmer: [bin_zb/ZG204ZV2-TH_zb0018.bin](../bin_zb/ZG204ZV2-TH_zb0018.bin)
- Zigbee with BLE OTA, OTA image: [bin_zb/1141-0238-00183001-ZG204ZV2-TH.zigbee](../bin_zb/1141-0238-00183001-ZG204ZV2-TH.zigbee)

[bin/README.md](../bin/README.md) explains the file names, including the images that convert a device from its stock firmware.
