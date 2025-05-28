#pragma once

#include <string>

#include "device/device.hpp"
#include "flow/flow.hpp"

namespace sim {

class IFlow;

enum PacketType { ACK, DATA };

struct Packet {
    Packet(PacketType a_type = PacketType::DATA, Size a_size_byte = 0,
           IFlow* a_flow = nullptr, Id a_source_id = 0, Id a_dest_id = 0, Time a_RTT = 0, Time a_send_time = 0);

    bool operator==(const Packet& packet) const;
    std::string to_string() const;

    std::uint32_t packet_num;
    PacketType type;
    Id source_id;
    Id dest_id;
    Time RTT;
    Size size_byte;
    IFlow* flow;
    Time send_time;
};

}  // namespace sim
