# nRF52840 integration

The portable protocol encoder/decoder is in `../common`. Integrate it into the OpenPuck nRF52840 firmware after its RF decoder has produced a complete controller state. Do not pass through OpenPuck's USB/XInput emulation layer: translate the decoded state directly into `seriouspuck::Input`, encode every 4 ms and send over a hardware UART.

UART target pins per the prototype: nRF P0.06 TX → RP2040 GP5 RX; nRF P0.08 RX ← RP2040 GP4 TX; common ground. Keep all UART pins at 3.3 V. Decode incoming `Rumble` packets and forward amplitudes to OpenPuck's haptics API. The OpenPuck source tree is an external dependency and is not vendored here yet, so exact callback and haptics API glue is board/version specific.
