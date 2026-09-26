# RP2040 integration

The portable protocol and mapper are in `../common` and `include/`. The GP2040-CE addon should feed each decoded, fresh input packet to `Mapper::map()` and apply the resulting `XboxState` through GP2040-CE's gamepad state API. Forward Xbox output rumble to the nRF as `MessageType::Rumble`.

Prototype pin plan: UART1 RX GP5 ← nRF P0.06 TX; UART1 TX GP4 → nRF P0.08 RX. Authentication is delegated to GP2040-CE Xbox passthrough with the original controller connected to the PIO USB host. The target must be integrated into a pinned GP2040-CE fork before it can be flashed; the host-buildable core deliberately avoids depending on unstable addon APIs.
