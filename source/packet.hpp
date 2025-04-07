#pragma once
#include <cstdint>
#include <string>

#include "device.hpp"
#include "flow.hpp"

namespace sim {

enum PacketType { ACK, DATA };

struct Packet {
    Packet(PacketType a_type = PacketType::DATA, std::uint32_t a_size = 0,
           IFlow* flow = nullptr);

    bool operator==(const Packet& packet) const;
    std::string to_string() const;

    PacketType type;
    std::uint32_t size;
    IFlow* flow;
};

}  // namespace sim
