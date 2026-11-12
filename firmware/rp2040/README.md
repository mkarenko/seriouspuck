# RP2040 integration

The portable protocol and mapper are in `../common` and `include/`. The GP2040-CE add-on should feed each decoded, fresh input packet to `Mapper::map()` and apply the resulting `XboxState` through GP2040-CE's gamepad state API. Forward Xbox output rumble to the nRF as `MessageType::Rumble`.

Board: Seeed XIAO RP2040. UART: **D6 (GP0) = TX, D7 (GP1) = RX** to the nRF52840's D7 and D6 (straight through). The USB host for the original pad is on D0 (GP26, D+) and D1 (GP27, D-). Authentication is done by GP2040-CE's Xbox One driver with the original controller on the PIO USB host.

The keyboard/mouse features that already work live in `../../patches/`. The add-on that receives the UART stream is not written yet.
