# MiaoMiaoCe MHO-C122

Battery Zigbee temperature and humidity sensor with an LCD.

> This page was generated from the board header and the build scripts. It has
> not been checked against the hardware.

## Summary

| | |
|---|---|
| Joins Zigbee as | `MiaoMiaoCe` / `MHO-C122-z` |
| Build name | `ZMHOC122` |
| Board define | `BOARD_MHO_C122` (id 11) |
| Board header | [src/board_mho_c122.h](../src/board_mho_c122.h) |
| Chip (header comment) | TLSR8251F512ET24 |
| Battery | CR2032 |
| Display | LCD, driver [src/lcd_mho_c122.c](../src/lcd_mho_c122.c) |
| Temperature / humidity sensor | SHT4x, SHTC3 (drivers enabled in this build) |
| Hardware notes (upstream) | <https://pvvx.github.io/MHO_C122> |

## Pins

Pin assignments the firmware uses for this build:

| Function | Pin |
|---|---|
| Button | PA5 (reset test point) |
| I2C (hardware) | PC2 and PC3 |

Notes from the board header. Where they disagree with the table above, the table is what the firmware uses:

```
 https://pvvx.github.io/MHO_C122

TLSR8251F512ET24

GPIO_PA5 - DM, free, (TRG)
GPIO_PA6 - DP, free, (RDS)
GPIO_PA7 - SWS, free, (debug TX)
GPIO_PB6 - used LCD, set "1"
GPIO_PB7 - free, (ADC1)
GPIO_PC2 - SDA, used I2C
GPIO_PC3 - SCL, used I2C
GPIO_PC4 - free
GPIO_PD2 - CS/PWM, free
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
make -s -j VERSION_BIN=_v0140 PROJECT_NAME=ZMHOC122 POJECT_DEF="-DBOARD=BOARD_MHO_C122" ZNAME="MiaoMiaoCe:MHO-C122-z"
```

Zigbee with BLE OTA (see [ZBdevices.md](../ZBdevices.md)):

```
make -s -j VERSION_BIN=_zb0018 PROJECT_NAME=ZMHOC122 USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_MHO_C122" ZNAME="MiaoMiaoCe:MHO-C122-z"
```

## Prebuilt firmware

- Zigbee only, for a hardware programmer: [bin/ZMHOC122_v0140.bin](../bin/ZMHOC122_v0140.bin)
- Zigbee only, OTA image: [bin/1141-020b-01403001-ZMHOC122.zigbee](../bin/1141-020b-01403001-ZMHOC122.zigbee)
- Zigbee with BLE OTA, for a hardware programmer: [bin_zb/ZMHOC122_zb0018.bin](../bin_zb/ZMHOC122_zb0018.bin)
- Zigbee with BLE OTA, OTA image: [bin_zb/1141-020b-00183001-ZMHOC122.zigbee](../bin_zb/1141-020b-00183001-ZMHOC122.zigbee)

[bin/README.md](../bin/README.md) explains the file names, including the images that convert a device from its stock firmware.
