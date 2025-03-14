#pragma once
#include <cstdint>

#include "device.hpp"
#include "flow.hpp"

namespace sim {

enum PacketType { ACK, DATA };

struct Packet {
    PacketType type;
    std::uint32_t size;
    IFlow* flow;
};

}  // namespace sim
