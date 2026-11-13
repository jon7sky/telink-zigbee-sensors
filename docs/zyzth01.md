# Tuya ZY-ZTH01

Battery Zigbee temperature and humidity sensor with an LCD.

Upstream description: Tuya ZY-ZTH02Pro/ZY-ZTH03Pro Zigbee LCD, 2 x AAA, SHT30/CHT832x

> This page was generated from the board header and the build scripts. It has
> not been checked against the hardware.

## Summary

| | |
|---|---|
| Joins Zigbee as | `Tuya` / `ZY-ZTH01-z` |
| Build name | `ZYZTH01` |
| Board define | `BOARD_ZYZTH01` (id 38) |
| Board header | [src/board_zyzth02pro.h](../src/board_zyzth02pro.h) |
| Chip (header comment) | TLSR8258 |
| Battery | 2 x AAA |
| Display | LCD, driver [src/lcd_zy_zth02pro.c](../src/lcd_zy_zth02pro.c) |
| Temperature / humidity sensor | CHT8215, SHT30 (drivers enabled in this build) |
| Hardware notes (upstream) | <https://pvvx.github.io/ZY-ZTH02Pro>, <https://pvvx.github.io/TS0201_TZ3000_v1w2k9dd> |

## Pins

Pin assignments the firmware uses for this build:

| Function | Pin |
|---|---|
| Button | PB4 |
| LED | PC2 |
| I2C (software) | SCL PC3, SDA PD2 |

Notes from the board header. Where they disagree with the table above, the table is what the firmware uses:

```
 https://pvvx.github.io/ZY-ZTH02Pro

TLSR8258

 GPIO_PA0 - free "RXD2" (Reed Switch, input)
 GPIO_PA1 - free "TXD2"
 GPIO_PA7 - SWS, (debug TX)
 GPIO_PB1 - free "TXD1"
 GPIO_PB4 - KEY
 GPIO_PB5 - free "P34" (R1 to VCC)
 GPIO_PB6 - free
 GPIO_PB7 - free "RXD1"
 GPIO_PC0 - free "PC0"
 GPIO_PC1 - free
 GPIO_PC2 - free "LED"
 GPIO_PC3 - SCL  "SCL"
 GPIO_PC4 - +BAT R11/R13
 GPIO_PD2 - SDA  "SDA"
 GPIO_PD3 - free
 GPIO_PD4 - free
 GPIO_PD7 - free
```

## Zigbee clusters

Set by `DEV_SERVICES` in the board header and [src/app_cfg.h](../src/app_cfg.h):

- Basic (`0x0000`), Power Configuration (`0x0001`), Poll Control (`0x0020`), OTA Upgrade (`0x0019`)
- Temperature Measurement (`0x0402`), Relative Humidity Measurement (`0x0405`), Thermostat User Interface Configuration (`0x0204`)

Button actions, settings and default reporting are the same for every board and are described in the [main README](../README.md).

## Build

Zigbee only:

```
make -s -j VERSION_BIN=_v0140 PROJECT_NAME=ZYZTH01 POJECT_DEF="-DBOARD=BOARD_ZYZTH01" ZNAME="Tuya:ZY-ZTH01-z"
```

Zigbee with BLE OTA (see [ZBdevices.md](../ZBdevices.md)):

```
make -s -j VERSION_BIN=_zb0018 PROJECT_NAME=ZYZTH01 USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_ZYZTH01" ZNAME="Tuya:ZY-ZTH01-z"
```

## Prebuilt firmware

- Zigbee only, for a hardware programmer: [bin/ZYZTH01_v0140.bin](../bin/ZYZTH01_v0140.bin)
- Zigbee only, OTA image: [bin/1141-0226-01403001-ZYZTH01.zigbee](../bin/1141-0226-01403001-ZYZTH01.zigbee)
- Zigbee with BLE OTA, for a hardware programmer: [bin_zb/ZYZTH01_zb0018.bin](../bin_zb/ZYZTH01_zb0018.bin)
- Zigbee with BLE OTA, OTA image: [bin_zb/1141-0226-00183001-ZYZTH01.zigbee](../bin_zb/1141-0226-00183001-ZYZTH01.zigbee)

[bin/README.md](../bin/README.md) explains the file names, including the images that convert a device from its stock firmware.
