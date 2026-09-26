#pragma once
#include "seriouspuck/protocol.hpp"

namespace seriouspuck {
enum class PadMode : uint8_t { Stick, Trackball, Flick };
struct MapperConfig {
    float gyro_sensitivity_x = 1.0f, gyro_sensitivity_y = 1.0f;
    float gyro_deadzone = 0.02f, smoothing = 0.25f;
    float left_pad_sensitivity = 1.0f, right_pad_sensitivity = 1.0f;
    PadMode left_pad = PadMode::Stick, right_pad = PadMode::Stick;
    bool gyro_enabled = false, gyro_on_right_touch = true, invert_y = true;
    uint32_t stale_timeout_us = 30'000;
};
struct XboxState {
    uint32_t buttons = 0;
    int16_t lx = 0, ly = 0, rx = 0, ry = 0;
    uint16_t lt = 0, rt = 0;
};

class Mapper {
public:
    explicit Mapper(MapperConfig config = {}) : config_(config) {}
    void set_config(const MapperConfig& config) { config_=config; }
    XboxState map(const Input& input, uint32_t now_us);
private:
    MapperConfig config_;
    float gyro_x_ = 0, gyro_y_ = 0;
};
} // namespace seriouspuck
