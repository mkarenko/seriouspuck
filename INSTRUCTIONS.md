# How to build it (stacked XIAO nRF52840 + XIAO RP2040)

> Status: this is a plan. The auth relay and the UART bridge have not been verified end to end. Check the Seeed pinouts before soldering.

![wiring](diagram.png)

Concept render of the finished build: `visual.png` (source `make_visual.py`).

Source of the diagram: `make_diagram.py` (`python3 make_diagram.py`, then `rsvg-convert -w 2240 diagram.svg -o diagram.png`).

## 1. Parts

| Qty | Part | Notes |
| --- | --- | --- |
| 1 | Seeed XIAO nRF52840 | the plain one (not Sense / Plus) |
| 1-2 | Seeed XIAO RP2040 | take a spare |
| 1 | USB-A female breakout, 2.54 mm | VCC / D- / D+ / GND printed on it |
| 1 | original Xbox controller | auth only |
| 2 | 7-pin 2.54 mm headers (male + female) | for the stack |
| opt. | 2 x 15 kOhm resistors | D+/D- to GND, only if the pad is not detected |

## 2. Get the sources

```sh
git clone --recurse-submodules <this repo>
# or, inside the repo:
git submodule update --init --recursive
```

## 3. Flash OpenPuck on the nRF52840

1. Install Arduino IDE / arduino-cli with the Adafruit nRF52 core.
2. Open `openpuck/OpenPuck`, select a board matching the XIAO nRF52840 (the default in the project is `adafruit:nrf52:feather52840`; pick the XIAO variant if your core has one, and check the LED pins in the variant).
3. Double-tap reset to enter the bootloader and upload. The board then shows up as a Steam Controller Puck (28de:1304) and pairs with the controller as described in the OpenPuck README.
4. Do this **before** stacking, using the nRF52840's own USB-C.

## 4. Flash GP2040-CE on the RP2040

1. Build GP2040-CE from `GP2040-CE` (Pico SDK, board config for the XIAO RP2040; there is a `SeeedXIAORP2040` config) or take a release `.uf2`.
2. Hold BOOT while plugging in, copy the `.uf2` to the `RPI-RP2` drive.
3. Open the GP2040-CE web config (http://192.168.7.1; see the GP2040-CE docs for how to enter it on your board). Set:
   - USB host pins: D+ = GP26, D- = GP27 (D- is the next GPIO)
   - Input mode: Xbox One / Series (GIP)
4. Do this before stacking too.

## 5. Solder the stack

Pad order (USB-C at the top): left column D0..D6, right column 5V, GND, 3V3, D10..D7.

| Between the boards | Notes |
| --- | --- |
| 5V <-> 5V | power for the nRF52840 comes from the RP2040 |
| GND <-> GND | common reference for UART |
| D6 <-> D6 | RP2040 TX (GP0) -> nRF52840 RX |
| D7 <-> D7 | nRF52840 TX -> RP2040 RX (GP1) |

Leave everything else unconnected. Do not connect 3V3 to 3V3.

Put the RP2040 at the bottom and the nRF52840 on top, with about 8-9 mm between them so the USB-C connectors do not collide. The nRF52840's USB-C stays unplugged from now on.

The UART pins in the nRF52840 firmware must be D6 = RX and D7 = TX (P1.12 TX, P1.11 RX), because the pads are wired straight through.

## 6. USB-A port for the original pad (on the RP2040 only)

| RP2040 pad | USB-A pin |
| --- | --- |
| 5V | VCC |
| D1 (GP27) | D- |
| D0 (GP26) | D+ |
| GND | GND |

Optional: 15 kOhm from D+ and from D- to GND.

Do not short GND against the 5V pad when probing; that is how the first board died.

## 7. Test, step by step

1. Plug the original pad into the USB-A port, plug the RP2040 into the Xbox. The console should recognise a controller.
2. Check that the console accepts the adapter (auth relay): the pad's own inputs do not control the console.
3. Power the stack, pair the Steam Controller with the OpenPuck, and verify packets over UART (see `docs/protocol.md`).
4. Map buttons: SC A -> Xbox A first, then the rest.

## 8. Known unknowns

- Seeed's wiki gives no voltage range for the 5V pad on the nRF52840 board.
- Pad order matches the official Seeed pinouts for both boards (see `visual_photos.png`).
- OpenPuck on the XIAO nRF52840 and the nRF -> RP2040 bridge code are not finished.
