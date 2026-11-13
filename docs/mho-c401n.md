# MiaoMiaoCe MHO-C401N

Battery Zigbee temperature and humidity sensor with an e-ink display.

Upstream description: 2022, SHTC3/SHT4X

> This page was generated from the board header and the build scripts. It has
> not been checked against the hardware.

## Summary

| | |
|---|---|
| Joins Zigbee as | `MiaoMiaoCe` / `MHO-C401N-z` |
| Build name | `ZMHOC401N` |
| Board define | `BOARD_MHO_C401N` (id 8) |
| Board header | [src/board_mho_c401n.h](../src/board_mho_c401n.h) |
| Chip (header comment) | TLSR8251F512ET24 |
| Battery | CR2032 |
| Display | E-ink, driver [src/epd_mho_c401n.c](../src/epd_mho_c401n.c) |
| Temperature / humidity sensor | SHT4x, SHTC3 (drivers enabled in this build) |
| Hardware notes (upstream) | <https://pvvx.github.io/MHO_C401N> |

## Pins

Pin assignments the firmware uses for this build:

| Function | Pin |
|---|---|
| Button | PB6 (Reed Switch, input, pcb mark "P5") |
| I2C (hardware) | PC2 and PC3 |
| E-ink display | BUSY PA5, RST PD7, RST2 PB7, CSB PA6, SDA PD2, SCL PC4 |

Notes from the board header. Where they disagree with the table above, the table is what the firmware uses:

```
 https://pvvx.github.io/MHO_C401N

TLSR8251F512ET24

GPIO_PA5 - used EPD_BUSY
GPIO_PA6 - used EPD_CSB
GPIO_PA7 - SWS, free, (debug TX)
GPIO_PB6 - used KEY, pcb mark "P5" (TRG)
GPIO_PB7 - used EPD_RST2
GPIO_PC2 - SDA, used I2C
GPIO_PC3 - SCL, used I2C
GPIO_PC4 - used EPD_SCL
GPIO_PD2 - used EPD_SDA
GPIO_PD7 - used EPD_RST
```

## Zigbee clusters

Set by `DEV_SERVICES` in the board header and [src/app_cfg.h](../src/app_cfg.h):

- Basic (`0x0000`), Power Configuration (`0x0001`), Poll Control (`0x0020`), OTA Upgrade (`0x0019`)
- Temperature Measurement (`0x0402`), Relative Humidity Measurement (`0x0405`), Thermostat User Interface Configuration (`0x0204`)

Button actions, settings and default reporting are the same for every board and are described in the [main README](../README.md).

## Build

Zigbee only:

```
make -s -j VERSION_BIN=_v0140 PROJECT_NAME=ZMHOC401N POJECT_DEF="-DBOARD=BOARD_MHO_C401N" ZNAME="MiaoMiaoCe:MHO-C401N-z"
```

Zigbee with BLE OTA (see [ZBdevices.md](../ZBdevices.md)):

```
make -s -j VERSION_BIN=_zb0018 PROJECT_NAME=ZMHOC401N USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_MHO_C401N" ZNAME="MiaoMiaoCe:MHO-C401N-z"
```

## Prebuilt firmware

- Zigbee only, for a hardware programmer: [bin/ZMHOC401N_v0140.bin](../bin/ZMHOC401N_v0140.bin)
- Zigbee only, OTA image: [bin/1141-0208-01403001-ZMHOC401N.zigbee](../bin/1141-0208-01403001-ZMHOC401N.zigbee)
- Zigbee with BLE OTA, for a hardware programmer: [bin_zb/ZMHOC401N_zb0018.bin](../bin_zb/ZMHOC401N_zb0018.bin)
- Zigbee with BLE OTA, OTA image: [bin_zb/1141-0208-00183001-ZMHOC401N.zigbee](../bin_zb/1141-0208-00183001-ZMHOC401N.zigbee)

[bin/README.md](../bin/README.md) explains the file names, including the images that convert a device from its stock firmware.
