#pragma once
#include <cstdint>
#include <string>

#include "device/device.hpp"
#include "flow.hpp"

namespace sim {

class IFlow;

enum PacketType { ACK, DATA };

struct Packet {
    Packet(PacketType a_type = PacketType::DATA, Size a_size = 0,
           std::weak_ptr<IFlow> a_flow = {});

    bool operator==(const Packet& packet) const;
    std::string to_string() const;
    std::shared_ptr<IRoutingDevice> get_destination() const;

    PacketType type;
    Size size;
    std::weak_ptr<IFlow> flow;
};

}  // namespace sim
