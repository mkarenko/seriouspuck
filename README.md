# SeriousPuck

Firmware for translating Steam Controller / Steam Controller 2 state from an nRF52840 OpenPuck receiver to an RP2040 GP2040-CE Xbox controller adapter.

## Current repository contents

- `firmware/common`: versioned, CRC protected UART protocol (COBS framing, 1 Mbaud default), input and rumble packets.
- `firmware/rp2040`: portable input mapper for sticks, touchpad-as-stick and gyro-to-right-stick.
- `firmware/nrf`: board integration notes and OpenPuck adapter boundary.
- `docs`: protocol and hardware notes.

The portable core builds with CMake. Board firmware integration depends on the upstream OpenPuck and GP2040-CE source trees and their board-specific APIs; this repository does not claim a flashable build until those upstream projects are pinned and the integration is compiled against them. Xbox Series authentication remains dependent on the GP2040-CE passthrough/auth implementation and compatible hardware.

## Build core

```sh
cmake -S . -B build
cmake --build build
```

See [SERIOUSPUCK.md](SERIOUSPUCK.md) for the complete architecture and wiring plan.
