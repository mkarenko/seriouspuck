#include "seriouspuck/mapper.hpp"
#include <algorithm>
#include <cmath>

namespace seriouspuck { namespace {
int16_t clamp_axis(float x) { return static_cast<int16_t>(std::clamp(x,-32767.0f,32767.0f)); }
int16_t pad_axis(int16_t axis, float sensitivity) { return clamp_axis(float(axis)*sensitivity); }
}
XboxState Mapper::map(const Input& in, uint32_t now) {
    XboxState out{};
    if (static_cast<uint32_t>(now-in.timestamp_us)>config_.stale_timeout_us) { gyro_x_=gyro_y_=0; return out; }
    out.buttons=in.buttons;
    out.lx=in.lx;out.ly=in.ly;out.rx=in.rx;out.ry=in.ry;out.lt=in.lt;out.rt=in.rt;
    // Touch coordinates are expected to be normalized to signed 16-bit by the receiver.
    if (in.buttons & LPadTouch) {
        if (config_.left_pad==PadMode::Stick) { out.lx=pad_axis(in.lpad_x,config_.left_pad_sensitivity);out.ly=pad_axis(in.lpad_y,config_.left_pad_sensitivity); }
    }
    if (in.buttons & RPadTouch) {
        if (config_.right_pad==PadMode::Stick) { out.rx=pad_axis(in.rpad_x,config_.right_pad_sensitivity);out.ry=pad_axis(in.rpad_y,config_.right_pad_sensitivity); }
        // Trackball and flick modes require release velocity/history; reserved for stateful extension.
    }
    const bool gyro_active=config_.gyro_enabled && (!config_.gyro_on_right_touch || (in.buttons&RPadTouch));
    if (!gyro_active) { gyro_x_=gyro_y_=0; return out; }
    float gx=float(in.gyro_y)/32768.0f, gy=float(in.gyro_x)/32768.0f;
    if (config_.invert_y) gy=-gy;
    const float magnitude=std::sqrt(gx*gx+gy*gy);
    if (magnitude<config_.gyro_deadzone) gx=gy=0;
    const float alpha=std::clamp(config_.smoothing,0.0f,1.0f);
    gyro_x_ += (gx-gyro_x_)*alpha; gyro_y_ += (gy-gyro_y_)*alpha;
    out.rx=clamp_axis(float(out.rx)+gyro_x_*config_.gyro_sensitivity_x*32767.0f);
    out.ry=clamp_axis(float(out.ry)+gyro_y_*config_.gyro_sensitivity_y*32767.0f);
    return out;
}
} // namespace seriouspuck
