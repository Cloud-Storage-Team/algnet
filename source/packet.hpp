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
           IFlow* flow = nullptr, Time a_sending_time = 0);

    bool operator==(const Packet& packet) const;
    std::string to_string() const;
    std::shared_ptr<IRoutingDevice> get_destination() const;

    PacketType type;
    Size size;
    IFlow* flow;
    Time sending_time;
};

}  // namespace sim
