#pragma once

#include <cstdint>
#include <vector>

namespace congestion::algorithms {

struct INTHeader {
    std::uint32_t ts;       // timestamp
    std::uint32_t q_len;    // queue length
    std::uint32_t tx_bytes; // transmitted bytes
    std::uint32_t b;        // link bandwidth capacity
};

struct Packet {
    Packet(std::uint32_t ts, const std::vector<INTHeader> &int_headers) : ts_send(ts), headers(int_headers) {}

    std::uint32_t ts_send;
    std::vector<INTHeader> headers;
};

class CongestionWindowHandler {
public:
    virtual void updateOnPacket(const Packet &packet, std::uint32_t ts) {};

    virtual std::uint32_t getCWND() const {};

    virtual std::uint32_t getRate() const {};
};

} // namespace congestion::algorithms