#pragma once
#include <cstdint>
#include <string>

#include "device/device.hpp"
#include "flow/flow.hpp"

namespace sim {

class IFlow;

enum PacketType { ACK, DATA };

struct Packet {
    Packet(PacketType a_type = PacketType::DATA, Size a_size_byte = 0,
           IFlow* flow = nullptr, Time a_send_time = 0);

    bool operator==(const Packet& packet) const;
    std::string to_string() const;
    std::shared_ptr<IRoutingDevice> get_destination() const;

    PacketType type;
    Size size_byte;
    IFlow* flow;
    Time send_time;
};

}  // namespace sim
