#pragma once
#include <cstdint>
#include <string>

#include "device/device.hpp"
#include "flow.hpp"

namespace sim {

class IFlow;

enum PacketType { ACK, DATA };

struct Packet {
    Packet(PacketType a_type = PacketType::DATA, Id a_source_id = 0, Id a_dest_id = 0, Time a_RTT = 0, Size a_size = 0,
           IFlow* a_flow = nullptr);

    bool operator==(const Packet& packet) const;
    std::string to_string() const;
    std::shared_ptr<IRoutingDevice> get_destination() const;

    PacketType type;
    Id source_id;
    Id dest_id;
    Time RTT;
    Size size;
    IFlow* flow;
};

}  // namespace sim
