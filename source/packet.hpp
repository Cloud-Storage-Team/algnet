#pragma once
#include <cstdint>

#include "device.hpp"
#include "flow.hpp"

namespace sim {

class IFlow;

enum PacketType { ACK, DATA };

struct Packet {
    // TODO: move implementation to .cpp or use existing if present
    Packet(PacketType a_type = PacketType::DATA, Size a_size = 0,
           IFlow* flow = nullptr);

    bool operator==(const Packet& packet) const;
    std::shared_ptr<IRoutingDevice> get_destination() const;

    PacketType type;
    Size size;
    IFlow* flow;
};

}  // namespace sim
