# Devices

One page per device this firmware supports. All of them are built from the same
source tree; the board is selected at build time with `-DBOARD=...`. The
[main README](../README.md) covers what is common to every board: flashing,
button actions, settings and OTA.

## Added in this fork

| Device | Joins Zigbee as | What it is | Build name |
|---|---|---|---|
| [HOBEIAN ZG-IR01](zg-ir01.md) | `HOBEIAN` / `ZG-IR01-z` | Battery IR blaster with a temperature and humidity sensor | `ZGIR01` |

## From upstream (pvvx/ZigbeeTLc)

These pages were generated from the board headers in `src/` and the build
scripts, and have not been checked against the hardware. The brand shown is the
manufacturer name the firmware reports, which is not always the name on the box.

| Device | Joins Zigbee as | What it is | Build name |
|---|---|---|---|
| [HOBEIAN ZG-223Z](zg223z.md) | `HOBEIAN` / `ZG-223Z-z` | Temperature, illuminance and rain sensor | `ZG223Z` |
| [MiaoMiaoCe MHO-C122](mho-c122.md) | `MiaoMiaoCe` / `MHO-C122-z` | Temperature and humidity sensor with an LCD | `ZMHOC122` |
| [MiaoMiaoCe MHO-C401](mho-c401.md) | `MiaoMiaoCe` / `MHO-C401-z` | Temperature and humidity sensor with an e-ink display | `ZMHOC401` |
| [MiaoMiaoCe MHO-C401N](mho-c401n.md) | `MiaoMiaoCe` / `MHO-C401N-z` | Temperature and humidity sensor with an e-ink display | `ZMHOC401N` |
| [Qingping CGDK2](cgdk2.md) | `Qingping` / `CGDK2-z` | Temperature and humidity sensor with an LCD | `ZCGDK2` |
| [Qingping CGG1](cgg1.md) | `Qingping` / `CGG1-z` | Temperature and humidity sensor with an e-ink display | `ZCGG1` |
| [Qingping CGG1N](cgg1n.md) | `Qingping` / `CGG1N-z` | Temperature and humidity sensor with an e-ink display | `ZCGG1N` |
| [Sonoff TH03](zth03.md) | `Sonoff` / `TH03-z` | Temperature and humidity sensor with an LCD | `TH03Z` |
| [Sonoff ZG-204ZL](zg204zl.md) | `Sonoff` / `ZG-204ZL-z` | Motion (PIR) and illuminance sensor | `ZG204ZL` |
| [Sonoff ZG-204ZS](zg204zs.md) | `Sonoff` / `ZG-204ZS-z` | Presence (PIR and RC2417 mmWave radar) and illuminance sensor | `ZG204ZS` |
| [Sonoff ZG-204ZV1](zg204zv1.md) | `Sonoff` / `ZG-204ZV1-z` | Presence (XBR818 radar) and illuminance sensor | `ZG204ZV1` |
| [Sonoff ZG-204ZV2-TH](zg204zv2-th.md) | `Sonoff` / `ZG-204ZV2-TH-z` | Temperature and humidity, presence (XBR818 radar) and illuminance sensor | `ZG204ZV2-TH` |
| [Sonoff ZG-204ZV2](zg204zv2.md) | `Sonoff` / `ZG-204ZV2-z` | Presence (XBR818 radar) and illuminance sensor | `ZG204ZV2` |
| [Sonoff ZG-204ZV](zg204zv.md) | `Sonoff` / `ZG-204ZV-z` | Temperature and humidity, presence (XBR818 radar) and illuminance sensor | `ZG204ZV` |
| [Sonoff ZG-303Z](zg303z.md) | `Sonoff` / `ZG-303Z-z` | Temperature and humidity and soil moisture sensor | `ZG303Z` |
| [Tuya KCTW1Z](kctw1z.md) | `Tuya` / `KCTW1Z-z` | Temperature and humidity sensor with an LCD | `KCTW1Z` |
| [Tuya LKTMZL02](lktmzl02.md) | `Tuya` / `LKTMZL02-z` | Temperature and humidity sensor with an LCD | `LKTMZL02Z` |
| [Tuya TH03Z](th03z.md) | `Tuya` / `TH03Z-z` | Temperature and humidity sensor | `ZTH03Z` |
| [Tuya TH05](zth05.md) | `Tuya` / `TH05-z` | Temperature and humidity sensor with an LCD | `ZTH05` |
| [Tuya TS0201](ts0201-tz3000.md) | `Tuya` / `TS0201-z` | Temperature and humidity sensor | `ZTS0201Z3000` |
| [Tuya TS202PIR1](ts0202-pir1.md) | `Tuya` / `TS202PIR1-z` | Motion (PIR) sensor | `TS202PIR1` |
| [Tuya TY0201](rsh-hs03.md) | `Tuya` / `TY0201-z` | Temperature and humidity sensor with an LCD | `ZTY0201` |
| [Tuya ZG-227Z](zg-227z.md) | `Tuya` / `ZG-227Z-z` | Temperature and humidity sensor | `ZG227Z` |
| [Tuya ZTH01](zth01.md) | `Tuya` / `ZTH01-z` | Temperature and humidity sensor | `ZTH01Z` |
| [Tuya ZTH02](zth02.md) | `Tuya` / `ZTH02-z` | Temperature and humidity sensor | `ZTH02Z` |
| [Tuya ZY-ZTH01](zyzth01.md) | `Tuya` / `ZY-ZTH01-z` | Temperature and humidity sensor with an LCD | `ZYZTH01` |
| [Tuya ZY-ZTH02](zyzth02.md) | `Tuya` / `ZY-ZTH02-z` | Temperature and humidity sensor | `ZYZTH02` |
| [Wing TS0201](ts0201wing.md) | `Wing` / `TS0201-z` | Temperature and humidity sensor with an LCD | `TS0201W` |
| [Xiaomi LYWSD03MMC](lywsd03mmc.md) | `Xiaomi` / `LYWSD03MMC-z` | Temperature and humidity sensor with an LCD | `Z03MMC` |
| [Xiaomi MJWSD06MMC](mjwsd06mmc.md) | `Xiaomi` / `MJWSD06MMC-z` | Temperature and humidity sensor with an LCD | `Z06MMC` |
| [ZBeacon MC](zb-mc.md) | `ZBeacon` / `MC-z` | Temperature and humidity sensor | `ZB_MC` |
| [ZBeacon TH01-2 (ZB2TH01 build)](zbeacon2-th01.md) | `ZBeacon` / `TH01-2-z` | Temperature and humidity sensor | `ZB2TH01` |
| [ZBeacon TH01-2 (ZB2TH01SHTC3 build)](zbeacon2-th01-shtc3.md) | `ZBeacon` / `TH01-2-z` | Temperature and humidity sensor | `ZB2TH01SHTC3` |
| [ZBeacon TH01](zbeacon-th01.md) | `ZBeacon` / `TH01-z` | Temperature and humidity sensor | `ZBTH01` |
| [ZBeacon TH03-ZBPS](th03-zbps.md) | `ZBeacon` / `TH03-ZBPS-z` | Temperature and humidity sensor with an LCD | `TH03ZBPS` |

`BOARD_CB3S` and `BOARD_TS0202_PIR2` also have board headers, but neither is
built by `WinMakeZ.cmd`, so they have no page.

## Adding a page

Copy [_template.md](_template.md) to `docs/<device>.md`, fill it in, and add a
row to the first table above. Pictures go in `docs/images/<device>/`.
