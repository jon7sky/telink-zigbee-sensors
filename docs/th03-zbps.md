# ZBeacon TH03-ZBPS

Battery Zigbee temperature and humidity sensor with an LCD.

Upstream description: Tuya TH03 Zigbee LCD, PCB TH03_ZBPS1.0, TLSR8656, LCD VKL060, GXHTV4 (SHT4x)

> This page was generated from the board header and the build scripts. It has
> not been checked against the hardware.

## Summary

| | |
|---|---|
| Joins Zigbee as | `ZBeacon` / `TH03-ZBPS-z` |
| Build name | `TH03ZBPS` |
| Board define | `BOARD_TH03_ZBPS` (id 59) |
| Board header | [src/board_th03_zbps.h](../src/board_th03_zbps.h) |
| Chip (header comment) | TLSR8656F512ET32 (TLSR825x) |
| Battery | CR2032 |
| Display | LCD, driver [src/lcd_zth03.c](../src/lcd_zth03.c) |
| Temperature / humidity sensor | SHT4x |
| Hardware notes (upstream) | <https://github.com/TheMysticle/ATC_MiThermometer/blob/th03-zbps1.0-support/TH03_Hardware_Info.md>, <https://github.com/pvvx/ATC_MiThermometer/discussions/791> |

## Pins

Pin assignments the firmware uses for this build:

| Function | Pin |
|---|---|
| Button | PD3 |
| I2C (software) | SCL PD4, SDA PA0 |
| I2C for the display | SCL PA1, SDA PD7 |

Notes from the board header. Where they disagree with the table above, the table is what the firmware uses:

```
TLSR8656F512ET32 (TLSR825x)

GPIO_PA0 - SDA, used I2C Sensor (GXHTV4 = SHT4x clone)
GPIO_PA1 - SCL, used I2C LCD (VKL060)
GPIO_PA7 - SWS
GPIO_PB0 - Sensor power ("1") + VBAT ADC
GPIO_PC0 - DO NOT DRIVE (TheMysticle: driving or configuring it as I/O crashes the board)
GPIO_PC1 - DO NOT DRIVE (same as PC0)
GPIO_PD3 - KEY (to GND)
GPIO_PD4 - SCL, used I2C Sensor
GPIO_PD7 - SDA, used I2C LCD

P2 header (top to bottom): GND, SWS, +3.3V

LCD: VKL060 (BL55028 compatible commands), I2C addr 0x3E, display RAM starts at 0x0B,
```

## Zigbee clusters

Set by `DEV_SERVICES` in the board header and [src/app_cfg.h](../src/app_cfg.h):

- Basic (`0x0000`), Power Configuration (`0x0001`), Poll Control (`0x0020`), OTA Upgrade (`0x0019`)
- Temperature Measurement (`0x0402`), Relative Humidity Measurement (`0x0405`), Thermostat User Interface Configuration (`0x0204`)

Button actions, settings and default reporting are the same for every board and are described in the [main README](../README.md).

## Build

Zigbee only:

```
make -s -j VERSION_BIN=_v0140 PROJECT_NAME=TH03ZBPS POJECT_DEF="-DBOARD=BOARD_TH03_ZBPS" ZNAME="ZBeacon:TH03-ZBPS-z"
```

Zigbee with BLE OTA (see [ZBdevices.md](../ZBdevices.md)):

```
make -s -j VERSION_BIN=_zb0018 PROJECT_NAME=TH03ZBPS USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_TH03_ZBPS" ZNAME="ZBeacon:TH03-ZBPS-z"
```

## Prebuilt firmware

- Zigbee only, for a hardware programmer: [bin/TH03ZBPS_v0140.bin](../bin/TH03ZBPS_v0140.bin)
- Zigbee only, OTA image: [bin/1141-023b-01403001-TH03ZBPS.zigbee](../bin/1141-023b-01403001-TH03ZBPS.zigbee)
- Zigbee with BLE OTA, for a hardware programmer: [bin_zb/TH03ZBPS_zb0018.bin](../bin_zb/TH03ZBPS_zb0018.bin)
- Zigbee with BLE OTA, OTA image: [bin_zb/1141-023b-00183001-TH03ZBPS.zigbee](../bin_zb/1141-023b-00183001-TH03ZBPS.zigbee)

[bin/README.md](../bin/README.md) explains the file names, including the images that convert a device from its stock firmware.
