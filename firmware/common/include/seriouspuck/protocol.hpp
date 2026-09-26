#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace seriouspuck {

constexpr uint8_t kVersion = 1;
constexpr uint32_t kDefaultBaud = 1'000'000;
constexpr std::size_t kMaxEncodedFrame = 128;

enum class MessageType : uint8_t { Input = 1, Rumble = 2, Status = 3 };

enum Button : uint32_t {
    A = 1u << 0, B = 1u << 1, X = 1u << 2, Y = 1u << 3,
    LB = 1u << 4, RB = 1u << 5, L3 = 1u << 6, R3 = 1u << 7,
    Menu = 1u << 8, View = 1u << 9,
    DpadUp = 1u << 10, DpadDown = 1u << 11,
    DpadLeft = 1u << 12, DpadRight = 1u << 13,
    L4 = 1u << 14, L5 = 1u << 15, R4 = 1u << 16, R5 = 1u << 17,
    LPadClick = 1u << 18, RPadClick = 1u << 19,
    LPadTouch = 1u << 20, RPadTouch = 1u << 21,
    LTClick = 1u << 22, RTClick = 1u << 23,
    LGripTouch = 1u << 24, RGripTouch = 1u << 25,
};

struct Input {
    uint32_t buttons = 0;
    int16_t lx = 0, ly = 0, rx = 0, ry = 0;
    uint16_t lt = 0, rt = 0;
    int16_t lpad_x = 0, lpad_y = 0, lpad_pressure = 0;
    int16_t rpad_x = 0, rpad_y = 0, rpad_pressure = 0;
    int16_t gyro_x = 0, gyro_y = 0, gyro_z = 0;
    int16_t accel_x = 0, accel_y = 0, accel_z = 0;
    uint32_t timestamp_us = 0;
};

struct Rumble { uint16_t left = 0, right = 0; uint8_t duration_ms = 0; };
struct Packet { MessageType type = MessageType::Input; uint16_t sequence = 0; Input input{}; Rumble rumble{}; uint8_t status = 0; };

// Wire format is [COBS(header + payload + CRC16)][0]. Integers are little-endian.
// CRC-16/CCITT-FALSE covers the header and payload.
std::size_t encode(const Packet& packet, uint8_t* out, std::size_t capacity);

class Decoder {
public:
    bool push(uint8_t byte, Packet& packet);
    void reset() { size_ = 0; dropping_ = false; }
private:
    std::array<uint8_t, kMaxEncodedFrame> buffer_{};
    std::size_t size_ = 0;
    bool dropping_ = false;
};

} // namespace seriouspuck
