# SeriousPuck UART protocol v1

UART is full duplex, 3.3 V logic, 1,000,000 baud, 8-N-1. The nRF sends input frames every 4 ms; the RP2040 sends rumble frames as needed.

Each frame is COBS-encoded and terminated by `00`. The decoded bytes are:

| Offset | Size | Field |
|---:|---:|---|
| 0 | 1 | protocol version (`1`) |
| 1 | 1 | type: input `1`, rumble `2`, status `3` |
| 2 | 2 | sequence number, little endian |
| 4 | 2 | payload size, little endian |
| 6 | N | payload |
| 6+N | 2 | CRC-16/CCITT-FALSE, little endian, over header and payload |

Input payload is 44 bytes: `buttons:u32`, four signed axes `i16`, two triggers `u16`, six touchpad values `i16`, gyro XYZ `i16`, acceleration XYZ `i16`, timestamp in microseconds `u32`. Buttons and all multibyte values are little endian. Axes and touchpad coordinates use signed range -32767..32767; triggers and pressure use 0..65535.

Rumble payload is `left:u16, right:u16, duration_ms:u8`; status payload is a single status byte. Unknown versions, types, lengths, or invalid CRCs are discarded. Sequence wrap is allowed. The receiver should expire input after 30 ms and neutralize outputs.
