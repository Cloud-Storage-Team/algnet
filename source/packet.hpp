#pragma once
#include <cstdint>

#include "device.hpp"
#include "flow.hpp"

namespace sim {

enum PacketType { ACK, DATA };

struct Packet {
    // TODO: move implementation to .cpp or use existing if present
    Packet(PacketType a_type = PacketType::DATA, std::uint32_t a_size = 0,
           Flow* a_flow = nullptr)
        : type(a_type), size(a_size), flow(a_flow){};

    bool operator==(const Packet& packet) const {
        return type == packet.type && size == packet.size &&
               flow == packet.flow;
    };

    PacketType type;
    std::uint32_t size;
    Flow* flow;
};

}  // namespace sim
