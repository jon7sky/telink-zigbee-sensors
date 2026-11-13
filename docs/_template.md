# Brand Model

One or two sentences: what the device is, what it is sold as, and what this
firmware does with it.

![Front of the device](images/<device>/front.jpg)

## Summary

| | |
|---|---|
| Joins Zigbee as | `Manufacturer` / `Model-z` |
| Build name | `PROJECT_NAME` |
| Board define | `BOARD_...` (id N) |
| Board header | [src/board_....h](../src/board_....h) |
| Chip | |
| Battery | |
| Display | |
| Sensors | |

## Hardware

Photos of the board, how to open the case, and where the programming pads
(SWS, GND, 3V3) are.

## Pins

| Function | Pin |
|---|---|
| Button | |
| LED | |

## Zigbee clusters

Clusters, attributes and commands that are specific to this device.

## Build

```
make -s -j VERSION_BIN=_vNNNN PROJECT_NAME=... POJECT_DEF="-DBOARD=BOARD_..." ZNAME="Manufacturer:Model-z"
```

## Flashing

How to get the firmware onto the device the first time, and what to back up
before doing so.

## Hub setup

Drivers, quirks or converters for Hubitat, ZHA or Zigbee2MQTT.

## Known issues
