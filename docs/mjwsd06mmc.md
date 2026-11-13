# Xiaomi MJWSD06MMC

Battery Zigbee temperature and humidity sensor with an LCD.

> This page was generated from the board header and the build scripts. It has
> not been checked against the hardware.

## Summary

| | |
|---|---|
| Joins Zigbee as | `Xiaomi` / `MJWSD06MMC-z` |
| Build name | `Z06MMC` |
| Board define | `BOARD_MJWSD06MMC` (id 13) |
| Board header | [src/board_mjwsd06mmc.h](../src/board_mjwsd06mmc.h) |
| Chip (header comment) | TLSR8251F512ET24 |
| Battery | CR2450 |
| Display | LCD, driver [src/lcd_mjwsd06mmc.c](../src/lcd_mjwsd06mmc.c) |
| Temperature / humidity sensor | SHT4x |
| Hardware notes (upstream) | <https://pvvx.github.io/MJWSD06MMC> |

## Pins

Pin assignments the firmware uses for this build:

| Function | Pin |
|---|---|
| Button | PD2 |
| I2C (hardware) | PC2 and PC3 |

Notes from the board header. Where they disagree with the table above, the table is what the firmware uses:

```
 https://pvvx.github.io/MJWSD06MMC

Original Flash markup:
  0x00000 Old Firmware bin
  0x20000 OTA New bin storage Area
  0x74000 Pair & Security info (CFG_ADR_BIND)?
  0x76000 MAC address (CFG_ADR_MAC)
  0x77000 Customize freq_offset adjust cap value (CUST_CAP_INFO_ADDR)
  0x78000 Mijia key-code (blt.3.1ks7k7p8ocg00, bindkey, ...)
  0x7D000 SerialStr: "V1.3F3.0-4-FY-LB-S-G-TM-"
  0x80000 End Flash (FLASH_SIZE)

//#define MI_HW_VER_FADDR 0x7D000 // Mi HW version (DEVICE_LYWSD03MMC)

TLSR8251F512ET24
 GPIO_PA5 - DM, free, pcb mark "A5" (KEY)
 GPIO_PA6 - DP, free, pcb mark "A6"
 GPIO_PA7 - SWS, free, pcb mark "S" (debug TX)
 GPIO_PB6 - free, pcb mark "PB6"
 GPIO_PB7 - free, pcb mark "R"
 GPIO_PC2 - SDA, used I2C, pcb mark "SDA"
 GPIO_PC3 - SCL, used I2C, pcb mark "SCK"
 GPIO_PC4 - Vbat (+V-R/RC-GND)
 GPIO_PD2 - KEY to GND
 GPIO_PD7 - free, pcb mark "T"
```

## Zigbee clusters

Set by `DEV_SERVICES` in the board header and [src/app_cfg.h](../src/app_cfg.h):

- Basic (`0x0000`), Power Configuration (`0x0001`), Poll Control (`0x0020`), OTA Upgrade (`0x0019`)
- Temperature Measurement (`0x0402`), Relative Humidity Measurement (`0x0405`), Thermostat User Interface Configuration (`0x0204`)

Button actions, settings and default reporting are the same for every board and are described in the [main README](../README.md).

## Build

Zigbee only:

```
make -s -j VERSION_BIN=_v0140 PROJECT_NAME=Z06MMC POJECT_DEF="-DBOARD=BOARD_MJWSD06MMC" ZNAME="Xiaomi:MJWSD06MMC-z"
```

Zigbee with BLE OTA (see [ZBdevices.md](../ZBdevices.md)):

```
make -s -j VERSION_BIN=_zb0018 PROJECT_NAME=Z06MMC USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_MJWSD06MMC" ZNAME="Xiaomi:MJWSD06MMC-z"
```

## Prebuilt firmware

- Zigbee only, for a hardware programmer: [bin/Z06MMC_v0140.bin](../bin/Z06MMC_v0140.bin)
- Zigbee only, OTA image: [bin/1141-020d-01403001-Z06MMC.zigbee](../bin/1141-020d-01403001-Z06MMC.zigbee)
- Zigbee with BLE OTA, for a hardware programmer: [bin_zb/Z06MMC_zb0018.bin](../bin_zb/Z06MMC_zb0018.bin)
- Zigbee with BLE OTA, OTA image: [bin_zb/1141-020d-00183001-Z06MMC.zigbee](../bin_zb/1141-020d-00183001-Z06MMC.zigbee)

[bin/README.md](../bin/README.md) explains the file names, including the images that convert a device from its stock firmware.
