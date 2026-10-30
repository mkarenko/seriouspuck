Hi! This is for Xbox Series X|S owners who also want to use a Steam Controller.

I think I've found a way to make the Steam Controller work on an Xbox. I wanted to build it myself and only publish the repo once it worked, but I got stuck. I accidentally shorted my RP2040 board before I could even test the pad -> RP2040 -> console auth step. On top of that, I've been on the Steam Controller reservation list only since June 19, so I probably won't get the controller before Q1 2027. So I'm posting the plan now, in case someone gets there before me and can enjoy a Steam Controller on Xbox Series.

**Status: nothing here is tested end to end.** It is a plan built from existing, working pieces.

### The plan

The Steam Controller talks to an **OpenPuck** receiver. OpenPuck talks to **GP2040-CE**. The console is plugged into the RP2040's own USB-C port, and a second USB port (USB host) takes an original Xbox pad, which is used only for authentication.

1. Build an [OpenPuck](https://github.com/safijari/openpuck) receiver on an nRF52840. It acts as the Steam Controller's wireless dongle.
2. Next to it (stacked on top, in my design) put an RP2040 running [GP2040-CE](https://github.com/OpenStickCommunity/GP2040-CE). It presents itself to the console as an Xbox controller.
3. Connect the two boards with a UART (1 Mbaud, 3 wires including GND). The nRF52840 sends the decoded controller state, the RP2040 turns it into Xbox input.
4. Add a USB-A or USB-C female port to the RP2040 for an original Xbox controller. GP2040-CE can use a connected controller as an authentication relay (merged in [PR #1684](https://github.com/OpenStickCommunity/GP2040-CE/pull/1684)), so the console accepts the adapter. The pad's own inputs are not forwarded, it only does the auth.

Both boards are Seeed XIAO modules with the same 21 x 17.8 mm footprint and pad order, so they can be stacked. Only 5V, GND and the two UART lines are connected between them. The nRF52840 is powered from the RP2040's 5V pad, so only one USB cable (to the console) is needed.

### What you need

- Steam Controller
- Xbox Series X|S
- an original Xbox controller (for auth)
- a female USB-A (or USB-C) breakout for that controller
- OpenPuck receiver board: [Seeed XIAO nRF52840](https://www.seeedstudio.com/Seeed-XIAO-BLE-nRF52840-p-5201.html)
- RP2040 board: [Seeed XIAO RP2040](https://www.seeedstudio.com/XIAO-RP2040-v1-0-p-5026.html)
- 2.54 mm pin headers, a soldering iron and optionally two 15 k resistors

Wiring diagram and step-by-step instructions are in the attachment / repo (`reddit/diagram.png`, `reddit/INSTRUCTIONS.md`).