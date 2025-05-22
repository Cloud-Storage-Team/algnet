#pragma once
#include <cstdint>
#include <string>

#include "device/device.hpp"
#include "flow.hpp"

namespace sim {

class IFlow;

enum PacketType { ACK, DATA };

struct Packet {
    Packet(PacketType a_type = PacketType::DATA, Size a_size_byte = 0,
           IFlow* flow = nullptr);

    bool operator==(const Packet& packet) const;
    std::string to_string() const;
    std::shared_ptr<IRoutingDevice> get_destination() const;

    PacketType type;
    Size size_byte;
    IFlow* flow;
};

}  // namespace sim
