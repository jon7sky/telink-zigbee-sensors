# Xiaomi LYWSD03MMC

Battery Zigbee temperature and humidity sensor with an LCD.

> This page was generated from the board header and the build scripts. It has
> not been checked against the hardware.

## Summary

| | |
|---|---|
| Joins Zigbee as | `Xiaomi` / `LYWSD03MMC-z` |
| Build name | `Z03MMC` |
| Board define | `BOARD_LYWSD03MMC` (id 10) |
| Board header | [src/board_lyws03mmc.h](../src/board_lyws03mmc.h) |
| Chip (header comment) | TLSR8251F512ET24 |
| Battery | CR2032 |
| Display | LCD, driver [src/lcd_lywsd03mmc.c](../src/lcd_lywsd03mmc.c) |
| Temperature / humidity sensor | SHT4x, SHTC3 (drivers enabled in this build) |
| Hardware notes (upstream) | <https://pvvx.github.io/ATC_MiThermometer/> |

## Pins

Pin assignments the firmware uses for this build:

| Function | Pin |
|---|---|
| Button | PA5 (reset test point) |
| I2C (hardware) | PC2 and PC3 |

Notes from the board header. Where they disagree with the table above, the table is what the firmware uses:

```
 TLSR8251F512ET24

GPIO_PA5 - DM, free, pcb mark "reset" (KEY)
GPIO_PA6 - DP, free, pcb mark "P8" (RDS)
GPIO_PA7 - SWS, free, pcb mark "P14", (debug TX)
GPIO_PB6 - used LCD, set "1"
GPIO_PB7 - free, pcb mark "B1" (ADC1)
GPIO_PC2 - SDA, used I2C, pcb mark "P12"
GPIO_PC3 - SCL, used I2C, pcb mark "P15"
GPIO_PC4 - free, pcb mark "P9" (ADC2)
GPIO_PD2 - CS/PWM, free (TRG)
GPIO_PD7 - free [B1.4], UART TX LCD [B1.6], pcb mark "P7"
```

## Zigbee clusters

Set by `DEV_SERVICES` in the board header and [src/app_cfg.h](../src/app_cfg.h):

- Basic (`0x0000`), Power Configuration (`0x0001`), Poll Control (`0x0020`), OTA Upgrade (`0x0019`)
- Temperature Measurement (`0x0402`), Relative Humidity Measurement (`0x0405`), Thermostat User Interface Configuration (`0x0204`)

Button actions, settings and default reporting are the same for every board and are described in the [main README](../README.md).

## Build

Zigbee only:

```
make -s -j VERSION_BIN=_v0140 PROJECT_NAME=Z03MMC POJECT_DEF="-DBOARD=BOARD_LYWSD03MMC" ZNAME="Xiaomi:LYWSD03MMC-z"
```

Zigbee with BLE OTA (see [ZBdevices.md](../ZBdevices.md)):

```
make -s -j VERSION_BIN=_zb0018 PROJECT_NAME=Z03MMC USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_LYWSD03MMC" ZNAME="Xiaomi:LYWSD03MMC-z"
```

## Prebuilt firmware

- Zigbee only, for a hardware programmer: [bin/Z03MMC_v0140.bin](../bin/Z03MMC_v0140.bin)
- Zigbee only, OTA image: [bin/1141-020a-01403001-Z03MMC.zigbee](../bin/1141-020a-01403001-Z03MMC.zigbee)
- Zigbee with BLE OTA, for a hardware programmer: [bin_zb/Z03MMC_zb0018.bin](../bin_zb/Z03MMC_zb0018.bin)
- Zigbee with BLE OTA, OTA image: [bin_zb/1141-020a-00183001-Z03MMC.zigbee](../bin_zb/1141-020a-00183001-Z03MMC.zigbee)

[bin/README.md](../bin/README.md) explains the file names, including the images that convert a device from its stock firmware.
