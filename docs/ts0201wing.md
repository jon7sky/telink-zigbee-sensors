# Wing TS0201

Battery Zigbee temperature and humidity sensor with an LCD.

Upstream description: TS0201_TZ3000_dnpd6ayp, Tuya Zigbee "Temp & Humidity Sensor", Wing TS0201 2xAAA

> This page was generated from the board header and the build scripts. It has
> not been checked against the hardware.

## Summary

| | |
|---|---|
| Joins Zigbee as | `Wing` / `TS0201-z` |
| Build name | `TS0201W` |
| Board define | `BOARD_TS0201WING` (id 52) |
| Board header | [src/board_ts0201_wing.h](../src/board_ts0201_wing.h) |
| Chip (header comment) | TLSR8258 512K Flash |
| Battery | 2 x AAA |
| Display | LCD, driver [src/lcd_zy_zth02pro.c](../src/lcd_zy_zth02pro.c) |
| Temperature / humidity sensor | SHT30 |
| Hardware notes (upstream) | <https://pvvx.github.io/TS0201_TZ3000_dnpd6ayp/> |

## Pins

Pin assignments the firmware uses for this build:

| Function | Pin |
|---|---|
| Button | PB4 |
| LED | PC2 |
| I2C (software) | SCL PC3, SDA PD2 |

Notes from the board header. Where they disagree with the table above, the table is what the firmware uses:

```
 https://pvvx.github.io/TS0201_TZ3000_dnpd6ayp/

TLSR8258 512K Flash

 GPIO_PA0 - free (Reed Switch, input)
 GPIO_PA7 - SWS, (debug TX)
 GPIO_PB4 - KEY
 GPIO_PC2 - LED
 GPIO_PC3 - SCL
 GPIO_PD2 - SDA
```

## Zigbee clusters

Set by `DEV_SERVICES` in the board header and [src/app_cfg.h](../src/app_cfg.h):

- Basic (`0x0000`), Power Configuration (`0x0001`), Poll Control (`0x0020`), OTA Upgrade (`0x0019`)
- Temperature Measurement (`0x0402`), Relative Humidity Measurement (`0x0405`), Thermostat User Interface Configuration (`0x0204`)

Button actions, settings and default reporting are the same for every board and are described in the [main README](../README.md).

## Build

Zigbee only:

```
make -s -j VERSION_BIN=_v0140 PROJECT_NAME=TS0201W POJECT_DEF="-DBOARD=BOARD_TS0201WING" ZNAME="Wing:TS0201-z"
```

Zigbee with BLE OTA (see [ZBdevices.md](../ZBdevices.md)):

```
make -s -j VERSION_BIN=_zb0018 PROJECT_NAME=TS0201W USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_TS0201WING" ZNAME="Wing:TS0201-z"
```

## Prebuilt firmware

- Zigbee only, for a hardware programmer: [bin/TS0201W_v0140.bin](../bin/TS0201W_v0140.bin)
- Zigbee only, OTA image: [bin/1141-0234-01403001-TS0201W.zigbee](../bin/1141-0234-01403001-TS0201W.zigbee)
- Zigbee with BLE OTA, for a hardware programmer: [bin_zb/TS0201W_zb0018.bin](../bin_zb/TS0201W_zb0018.bin)
- Zigbee with BLE OTA, OTA image: [bin_zb/1141-0234-00183001-TS0201W.zigbee](../bin_zb/1141-0234-00183001-TS0201W.zigbee)

[bin/README.md](../bin/README.md) explains the file names, including the images that convert a device from its stock firmware.
