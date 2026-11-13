# Qingping CGDK2

Battery Zigbee temperature and humidity sensor with an LCD.

> This page was generated from the board header and the build scripts. It has
> not been checked against the hardware.

## Summary

| | |
|---|---|
| Joins Zigbee as | `Qingping` / `CGDK2-z` |
| Build name | `ZCGDK2` |
| Board define | `BOARD_CGDK2` (id 6) |
| Board header | [src/board_cgdk2.h](../src/board_cgdk2.h) |
| Chip (header comment) | TLSR8253F512ET32 |
| Battery | CR2430 |
| Display | LCD, driver [src/lcd_cgdk2.c](../src/lcd_cgdk2.c) |
| Temperature / humidity sensor | SHT4x, SHTC3 (drivers enabled in this build) |
| Hardware notes (upstream) | <https://pvvx.github.io/CGDK2> |

## Pins

Pin assignments the firmware uses for this build:

| Function | Pin |
|---|---|
| Button | PC4 |
| I2C (hardware) | PC0 and PC1 |

Notes from the board header. Where they disagree with the table above, the table is what the firmware uses:

```
 https://pvvx.github.io/CGDK2

TLSR8253F512ET32

GPIO_PA0 - free, UART_RX, pcb mark "TP3", (Reed Switch, input)
GPIO_PA1 - free
GPIO_PA7 - SWS, free, (debug TX), pcb mark "TP17"
GPIO_PB1 - free, UART_TX, pcb mark "TP1", (TRG)
GPIO_PB4 - free
GPIO_PB5 - free
GPIO_PB6 - free, (ADC2)
GPIO_PB7 - free, (ADC1)
GPIO_PC0 - SDA, used I2C
GPIO_PC1 - SCL, used I2C
GPIO_PC2 - free
GPIO_PC3 - free
GPIO_PC4 - used KEY (RDS)
GPIO_PD2 - free
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
make -s -j VERSION_BIN=_v0140 PROJECT_NAME=ZCGDK2 POJECT_DEF="-DBOARD=BOARD_CGDK2" ZNAME="Qingping:CGDK2-z"
```

Zigbee with BLE OTA (see [ZBdevices.md](../ZBdevices.md)):

```
make -s -j VERSION_BIN=_zb0018 PROJECT_NAME=ZCGDK2 USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_CGDK2" ZNAME="Qingping:CGDK2-z"
```

## Prebuilt firmware

- Zigbee only, for a hardware programmer: [bin/ZCGDK2_v0140.bin](../bin/ZCGDK2_v0140.bin)
- Zigbee only, OTA image: [bin/1141-0206-01403001-ZCGDK2.zigbee](../bin/1141-0206-01403001-ZCGDK2.zigbee)
- Zigbee with BLE OTA, for a hardware programmer: [bin_zb/ZCGDK2_zb0018.bin](../bin_zb/ZCGDK2_zb0018.bin)
- Zigbee with BLE OTA, OTA image: [bin_zb/1141-0206-00183001-ZCGDK2.zigbee](../bin_zb/1141-0206-00183001-ZCGDK2.zigbee)

[bin/README.md](../bin/README.md) explains the file names, including the images that convert a device from its stock firmware.
