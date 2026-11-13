# Tuya KCTW1Z

Battery Zigbee temperature and humidity sensor with an LCD.

Upstream description: Tuya KCTW1Z / Moes ZSS-KB-TH, ZigBee LCD, SHT40, CR2032

> This page was generated from the board header and the build scripts. It has
> not been checked against the hardware.

## Summary

| | |
|---|---|
| Joins Zigbee as | `Tuya` / `KCTW1Z-z` |
| Build name | `KCTW1Z` |
| Board define | `BOARD_KCTW1Z` (id 61) |
| Board header | [src/board_kctw1z.h](../src/board_kctw1z.h) |
| Chip (header comment) | ZTU module pinout |
| Battery | CR2032 |
| Display | LCD, driver [src/lcd_kctw1z.c](../src/lcd_kctw1z.c) |
| Temperature / humidity sensor | CHT8305, AHT20/AHT30, SHT4x, SHT30 (drivers enabled in this build) |

## Pins

Pin assignments the firmware uses for this build:

| Function | Pin |
|---|---|
| Button | PC0 |
| I2C (hardware) | PC2 and PC3 |
| I2C for the display | SCL PC1, SDA PD2 |

Notes from the board header. Where they disagree with the table above, the table is what the firmware uses:

```
Tuya KCTW1Z (_TZ3000_itnrsufe, TS0201) = Moes ZSS-KB-TH
         Zigbee Temperature/Humidity Sensor with LCD, CR2032

ZTU module pinout:
  ZTU pin 3  -> PC0 : BUTTON (active LOW -> GND)
  ZTU pin 8  -> PC2 : I2C SCL (SHT40 @ 0x44)
  ZTU pin 9  -> PC3 : I2C SDA (SHT40 @ 0x44)
  ZTU C1     -> LCD SCL (VINKA VKL144B)
  ZTU D2     -> LCD SDA (VINKA VKL144B)
  ZTU pin 13 -> GND
  ZTU pin 14 -> VCC
  ZTU pin 15 -> PB1 : UART TX (ISP)
  ZTU pin 16 -> PB7 : UART RX (ISP)
  ZTU pin 18 -> RST : RESET (ISP)
  ZTU pin 4  -> SWS : SWire programming (ISP)

LED: NOT present. Identify via Zigbee icon on LCD.
```

## Zigbee clusters

Set by `DEV_SERVICES` in the board header and [src/app_cfg.h](../src/app_cfg.h):

- Basic (`0x0000`), Power Configuration (`0x0001`), Poll Control (`0x0020`), OTA Upgrade (`0x0019`)
- Temperature Measurement (`0x0402`), Relative Humidity Measurement (`0x0405`), Thermostat User Interface Configuration (`0x0204`)

Button actions, settings and default reporting are the same for every board and are described in the [main README](../README.md).

## Build

Zigbee only:

```
make -s -j VERSION_BIN=_v0140 PROJECT_NAME=KCTW1Z POJECT_DEF="-DBOARD=BOARD_KCTW1Z" ZNAME="Tuya:KCTW1Z-z"
```

There is no Zigbee with BLE OTA build of this board in `WinMakeZB.cmd`.

## Prebuilt firmware

- Zigbee only, for a hardware programmer: [bin/KCTW1Z_v0140.bin](../bin/KCTW1Z_v0140.bin)
- Zigbee only, OTA image: [bin/1141-023d-01403001-KCTW1Z.zigbee](../bin/1141-023d-01403001-KCTW1Z.zigbee)

[bin/README.md](../bin/README.md) explains the file names, including the images that convert a device from its stock firmware.
