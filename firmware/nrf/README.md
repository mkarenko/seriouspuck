# nRF52840 integration

The portable protocol encoder/decoder is in `../common`. Integrate it into the OpenPuck firmware after its RF decoder has produced a complete controller state. Do not pass through OpenPuck's USB/XInput emulation: translate the decoded state directly into `seriouspuck::Input`, encode every 4 ms and send it over a hardware UART.

Board: Seeed XIAO nRF52840. UART pins: **D6 (P1.11) = RX, D7 (P1.12) = TX**, wired straight through to the RP2040's D6 (GP0, TX) and D7 (GP1, RX), plus common GND. Keep all UART pins at 3.3 V. Decode incoming `Rumble` packets and forward amplitudes to OpenPuck's haptics API.

OpenPuck is an external dependency (a separate checkout, see the README for the pinned commit). It has not been built for the XIAO nRF52840 yet, so the exact callback and haptics glue is board and version specific.
