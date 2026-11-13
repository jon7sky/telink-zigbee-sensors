# HOBEIAN ZG-IR01 battery IR blaster

Board `BOARD_ZG_IR01` (fork-local, not in upstream ZigbeeTLc): a battery Zigbee
IR blaster with a temperature / humidity sensor, sold as `_TZE200_33rdmvgw` /
`ZG-IR01`. With this firmware it joins as `HOBEIAN` / `ZG-IR01-z`.

The point of the firmware is IR codes of any protocol and any practical length.
The stock firmware decodes and validates each frame and moves codes to the hub
in chunks, so long codes (a Mitsubishi mini-split sends a 144-bit frame twice,
about 580 timings) could not be learned or sent.

## Hardware

TLSR8258 (marked `Z2`), 1M flash, 2 x AAA. Pin map in
[src/board_zg_ir01.h](../src/board_zg_ir01.h). Test pads, counting from the square
one: 1 = PB7 (UART RX), 2 = PB1 (UART TX), 3 = SWS, 4 = GND, 5 = +3V. The stock
firmware uses the UART at 115200; this firmware does not use it.

## IR codes

A code is a list of raw mark / space durations, recorded from the IR receiver
and replayed on a 38 kHz carrier. Nothing is decoded.

- Up to 2000 timings per code, 4 us resolution.
- Capture ends after 120 ms of silence, so the gap between repeated frames is
  recorded as part of the code.
- Codes are stored on the device, one 4K flash sector per slot, and never cross
  the radio. The hub only names a slot.
- **64 slots. This is a choice, not the limit:** the free flash
  (`0x80000`..`0xFBFFF`) has room for 124. To raise it, change `IR_SLOT_COUNT`
  in [src/ir_blaster.h](../src/ir_blaster.h); above 64 the `slots used` attribute
  must become wider than a 64-bit bitmap.
- Each slot also stores the code's **name**, up to 64 bytes of any value (the
  hub sends UTF-8; spaces are fine), so the hub can rebuild its list of names
  after a re-pair. Slot layout: header (8 bytes), timings from `0x008`, name
  length and name at `0xFB8`. A slot written before names existed has an
  erased name area, which reads as "no name".
- Slots survive a Zigbee reset and a factory reset. They need a 1M flash; on a
  smaller one every command answers "no storage".

## Zigbee interface

Cluster `0xFC49` on endpoint 1, no manufacturer code. Commands are attribute
writes of a slot number.

| Attribute | Type | Access | Meaning |
|---|---|---|---|
| `0x0000` learn | uint8 | RW | Write a slot to learn into it. `0x80` cancels. |
| `0x0001` send | uint8 | RW | Write a slot to transmit it. |
| `0x0002` erase | uint8 | RW | Write a slot to erase it. `0x80` erases all. |
| `0x0003` result | uint32 | R, reported | `status \| slot << 8 \| timings << 16`, sent to the coordinator after every operation. |
| `0x0004` slots used | bitmap64 | R | Bit n set: slot n holds a code. |
| `0x0005` slot count | uint8 | R | 64 |
| `0x0006` idle poll | uint8 | RW | Idle poll interval in 1/4 s, 1..40, saved. Default 4 (1 s). |

Names travel in cluster commands rather than attributes, so the firmware can
check their length against the frame (the stack copies string attributes
without a length check). A name is `[length][bytes]`, at most 64 bytes.

| Command | Direction | Payload |
|---|---|---|
| `0x00` learn | hub to device | `[slot][name]`: learn into the slot and store the name with it |
| `0x01` rename | hub to device | `[slot][name]`: change the stored name (rewrites the slot) |
| `0x02` query | hub to device | `[slot]`, or `0x80` for every slot that holds a code |
| `0x00` slot info | device to hub | `[slot][timings, uint16][name]`, timings 0 = empty slot |

The device sends slot info for each queried slot, one message at a time, and
after every successful learn or rename.

Result status: 1 learning, 2 learned, 3 learn timeout (15 s), 4 code too long,
5 sent, 6 slot empty, 7 bad slot, 8 erased, 9 flash error, 10 no storage,
11 cancelled.

During a learn the LED blinks and the device stays awake. A failed learn leaves
the slot's previous code in place.

## Battery life and response time

The device sleeps between polls. It polls at the idle interval (1 s by
default) and every 250 ms for 15 s after any IR command, so the first command
of a group waits up to one interval and the following ones are near-immediate.
The sensor is still read at its own interval, not at every poll.

## Hubitat

Driver and Hubitat Package Manager manifest:
[jon7sky/hubitat-zg-ir01](https://github.com/jon7sky/hubitat-zg-ir01).

## Build and flash

```
make -s -j PROJECT_NAME=ZGIR01 POJECT_DEF="-DBOARD=BOARD_ZG_IR01" VERSION_BIN=_v0140 ZNAME="HOBEIAN:ZG-IR01-z"
```

Write the `.bin` at address 0 over SWire. Back up the whole flash first: the
stock image and the device's Tuya keys cannot be recovered otherwise.
