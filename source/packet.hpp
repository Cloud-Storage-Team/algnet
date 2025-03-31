#pragma once
#include <cstdint>

#include "device.hpp"
#include "flow.hpp"

namespace sim {

enum PacketType { ACK, DATA };

struct Packet {
    Packet(PacketType a_type = PacketType::DATA, std::uint32_t a_size = 0,
           IFlow* flow = nullptr);

    bool operator==(const Packet& packet) const;

    PacketType type;
    std::uint32_t size;
    IFlow* flow;
};

}  // namespace sim
