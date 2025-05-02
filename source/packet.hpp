#pragma once
#include <cstdint>
#include <string>

#include "flow.hpp"

namespace sim {

class IFlow;
class IRoutingDevice;

enum PacketType { ACK, DATA, CREDIT_REQUEST, CREDIT, CREDIT_STOP_P };

struct Packet {
    Packet(PacketType a_type = PacketType::DATA, Id a_source_id = 0, Id a_dest_id = 0, Time a_RTT = 0, Size a_size = 0,
           IFlow* a_flow = nullptr);

    bool operator==(const Packet& packet) const;
    std::string to_string() const;
    std::shared_ptr<IRoutingDevice> get_destination() const;

    std::uint32_t packet_num;
    PacketType type;
    Id source_id;
    Id dest_id;
    Time RTT;
    Size size;
    IFlow* flow;
};

}  // namespace sim
