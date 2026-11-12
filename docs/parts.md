# Parts

Final build with the stacked Seeed XIAO boards and an original Xbox Series pad for authentication.

| Qty | Part | Notes |
| --- | --- | --- |
| 1 | Seeed **XIAO nRF52840** | the plain one, not Sense or Plus. OpenPuck receiver. Not verified on XIAO yet, a spare is a good idea. |
| 1 (+1 spare) | Seeed **XIAO RP2040** | GP2040-CE. Same 21 x 17.8 mm footprint and pad order as the nRF board, so they stack. |
| 1 (+1 spare) | USB-A female breakout, 2.54 mm | for the original pad. Check that VCC / D- / D+ / GND are labelled. |
| 1-2 | 1x40 pin header, 2.54 mm | stack and wiring |
| 1 pack | Dupont jumper wires, **10 cm** | short wires matter for the USB data lines |
| - | original Xbox Series X|S controller | authentication only. USB-A to USB-C data cable. |
| - | USB-C to USB-A data cable | RP2040 to the console |
| - | soldering iron, solder, flux, heat shrink | |
| recommended | multimeter | the first RP2040 board died from a short between 5V and GND |
| optional | 2 x 15 kOhm resistors | D+ and D- to GND. The pad worked without them in the prototype. |
| optional | USB hub | pad + keyboard + mouse on one host port. A powered hub is safer, but a plain one worked. |

A USB-C female breakout instead of USB-A also works, but it needs two 56 kOhm resistors from CC1 and CC2 to 5V (so the pad sees a host) and a breakout **without** the 5.1 kOhm pull-downs on CC that most power-only boards carry. USB-A is simpler.

Seeed XIAO pinouts: [XIAO nRF52840](https://wiki.seeedstudio.com/XIAO_BLE/), [XIAO RP2040](https://wiki.seeedstudio.com/XIAO-RP2040/).
