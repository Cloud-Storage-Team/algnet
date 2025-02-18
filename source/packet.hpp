#pragma once
#include <cstdint>

namespace sim {

class Device;
class Flow;

enum PacketType { ACK, DATA };

struct Packet {
    PacketType type;
    std::uint32_t size;
    Device* src;
    Device* dest;
    Flow* flow;
};

}  // namespace sim
