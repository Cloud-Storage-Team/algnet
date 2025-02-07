#pragma once

#include "device.hpp"
#include "flow.hpp"

namespace sim {

enum PacketType { ACK, DATA };

struct Packet {
    PacketType type;
    int size;
    Device* src;
    Device* dest;
    Flow* flow;
};

}  // namespace sim
