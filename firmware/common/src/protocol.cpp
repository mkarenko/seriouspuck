#include "seriouspuck/protocol.hpp"

namespace seriouspuck { namespace {
struct Writer {
    uint8_t* p; std::size_t n = 0;
    void u8(uint8_t v) { p[n++] = v; }
    void u16(uint16_t v) { u8(v); u8(v >> 8); }
    void i16(int16_t v) { u16(static_cast<uint16_t>(v)); }
    void u32(uint32_t v) { u16(v); u16(v >> 16); }
};
struct Reader {
    const uint8_t* p; std::size_t n = 0;
    uint8_t u8() { return p[n++]; }
    uint16_t u16() { uint16_t a=u8(); return a | (uint16_t(u8()) << 8); }
    int16_t i16() { return static_cast<int16_t>(u16()); }
    uint32_t u32() { uint32_t a=u16(); return a | (uint32_t(u16()) << 16); }
};
uint16_t crc16(const uint8_t* data, std::size_t n) {
    uint16_t crc = 0xffff;
    while (n--) { crc ^= uint16_t(*data++) << 8; for (int i=0;i<8;i++) crc = crc & 0x8000 ? (crc << 1) ^ 0x1021 : crc << 1; }
    return crc;
}
std::size_t cobs_encode(const uint8_t* src, std::size_t n, uint8_t* dst, std::size_t cap) {
    if (!cap) return 0;
    std::size_t code_pos=0, write=1; uint8_t code=1;
    for (std::size_t i=0;i<n;i++) {
        if (src[i] == 0) { if (code_pos >= cap) return 0; dst[code_pos]=code; code_pos=write++; code=1; }
        else { if (write >= cap) return 0; dst[write++]=src[i]; if (++code == 0xff) { dst[code_pos]=code; code_pos=write++; code=1; } }
    }
    if (code_pos >= cap || write >= cap) return 0;
    dst[code_pos]=code; dst[write++]=0; return write;
}
bool cobs_decode(const uint8_t* src, std::size_t n, uint8_t* dst, std::size_t cap, std::size_t& out_n) {
    out_n=0; std::size_t i=0;
    while (i<n) { uint8_t code=src[i++]; if (!code || i + code - 1 > n) return false;
        for (uint8_t j=1;j<code;j++) { if (out_n>=cap) return false; dst[out_n++]=src[i++]; }
        if (code != 0xff && i<n) { if (out_n>=cap) return false; dst[out_n++]=0; }
    } return true;
}
void write_input(Writer& w, const Input& x) {
    w.u32(x.buttons); w.i16(x.lx);w.i16(x.ly);w.i16(x.rx);w.i16(x.ry);w.u16(x.lt);w.u16(x.rt);
    w.i16(x.lpad_x);w.i16(x.lpad_y);w.i16(x.lpad_pressure);w.i16(x.rpad_x);w.i16(x.rpad_y);w.i16(x.rpad_pressure);
    w.i16(x.gyro_x);w.i16(x.gyro_y);w.i16(x.gyro_z);w.i16(x.accel_x);w.i16(x.accel_y);w.i16(x.accel_z);w.u32(x.timestamp_us);
}
bool read_packet(const uint8_t* data, std::size_t n, Packet& p) {
    if (n<8 || data[0]!=kVersion) return false;
    const uint16_t expected=uint16_t(data[n-2]) | (uint16_t(data[n-1])<<8);
    if (crc16(data,n-2)!=expected) return false;
    p.type=static_cast<MessageType>(data[1]); Reader r{data+6};
    // Header: version, type, sequence LE, payload length LE.
    p.sequence=uint16_t(data[2]) | (uint16_t(data[3])<<8);
    const uint16_t len=uint16_t(data[4]) | (uint16_t(data[5])<<8);
    if (n != std::size_t(len)+8) return false;
    if (p.type==MessageType::Input && len==44) {
        p.input.buttons=r.u32(); p.input.lx=r.i16();p.input.ly=r.i16();p.input.rx=r.i16();p.input.ry=r.i16();p.input.lt=r.u16();p.input.rt=r.u16();
        p.input.lpad_x=r.i16();p.input.lpad_y=r.i16();p.input.lpad_pressure=r.i16();p.input.rpad_x=r.i16();p.input.rpad_y=r.i16();p.input.rpad_pressure=r.i16();
        p.input.gyro_x=r.i16();p.input.gyro_y=r.i16();p.input.gyro_z=r.i16();p.input.accel_x=r.i16();p.input.accel_y=r.i16();p.input.accel_z=r.i16();p.input.timestamp_us=r.u32(); return true;
    }
    if (p.type==MessageType::Rumble && len==5) { p.rumble.left=r.u16();p.rumble.right=r.u16();p.rumble.duration_ms=r.u8();return true; }
    if (p.type==MessageType::Status && len==1) { p.status=r.u8();return true; }
    return false;
}
} // namespace

std::size_t encode(const Packet& p, uint8_t* out, std::size_t cap) {
    uint8_t raw[64]{}; Writer w{raw}; w.u8(kVersion);w.u8(static_cast<uint8_t>(p.type));w.u16(p.sequence);
    const std::size_t len_at=w.n;w.u16(0);const std::size_t payload_at=w.n;
    if (p.type==MessageType::Input) write_input(w,p.input);
    else if (p.type==MessageType::Rumble) {w.u16(p.rumble.left);w.u16(p.rumble.right);w.u8(p.rumble.duration_ms);}
    else if (p.type==MessageType::Status) w.u8(p.status); else return 0;
    const uint16_t len=w.n-payload_at;raw[len_at]=len&0xff;raw[len_at+1]=len>>8;const uint16_t crc=crc16(raw,w.n);w.u16(crc);
    return cobs_encode(raw,w.n,out,cap);
}
bool Decoder::push(uint8_t byte, Packet& packet) {
    if (byte==0) {
        if (dropping_ || !size_) { size_=0;dropping_=false;return false; }
        uint8_t raw[64];std::size_t n=0;const bool ok=cobs_decode(buffer_.data(),size_,raw,sizeof(raw),n) && read_packet(raw,n,packet);
        size_=0;return ok;
    }
    if (dropping_) return false;
    if (size_==buffer_.size()) {size_=0;dropping_=true;return false;}
    buffer_[size_++]=byte;return false;
}
} // namespace seriouspuck
