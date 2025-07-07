#pragma once
#include <iostream>
#include <queue>

#include "device/interfaces/i_host.hpp"
#include "packet/packet.hpp"
#include "utils/identifier_factory.hpp"

namespace sim {

// Structure with common fields of BasicFlow and TcpFlow
struct FlowCommon {
    FlowCommon(Id a_id, std::shared_ptr<IHost> a_src,
               std::shared_ptr<IHost> a_dest, Size a_packet_size,
               Time a_delay_between_packets, std::uint32_t a_packets_to_send,
               Time a_delay_threshold = 4000);

    RoutingPacket generate_routing_packet() const;

    Id id;

    std::weak_ptr<IHost> src;
    std::weak_ptr<IHost> dest;
    Size packet_size;
    Time delay_between_packets;
    std::uint32_t packets_to_send;
    Time delay_threshold;

    std::uint32_t packets_acked;
    Size sent_bytes;

    std::queue<Packet> sending_buffer;
};

std::ostream& operator<<(std::ostream& out, const FlowCommon& flow_common);
}  // namespace sim