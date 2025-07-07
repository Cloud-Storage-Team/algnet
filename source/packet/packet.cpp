#include "packet/packet.hpp"

#include <sstream>

namespace sim {

Packet::Packet(Size a_size_byte, IFlow* a_flow, Id a_source_id, Id a_dest_id,
               Time a_sent_time, Size a_sent_bytes_at_origin,
               bool a_ecn_capable_transport, bool a_congestion_experienced)
    : Packet(RoutingPacket(a_size_byte, a_source_id, a_dest_id,
                           a_sent_bytes_at_origin, a_sent_time),
             a_flow, a_ecn_capable_transport, a_congestion_experienced) {}

Packet::Packet(RoutingPacket a_routing_packet, IFlow* a_flow,
               bool a_ecn_capable_transport, bool a_congestion_experienced)
    : RoutingPacket(std::move(a_routing_packet)),
      flags(0),
      flow(a_flow),
      ecn_capable_transport(a_ecn_capable_transport),
      congestion_experienced(a_congestion_experienced) {}

bool Packet::operator==(const Packet& packet) const {
    return flow == packet.flow && source_id == packet.source_id &&
           dest_id == packet.dest_id && size == packet.size &&
           flags == packet.flags;
}

// TODO: think about some ID for packet (currently its impossible to distinguish
// packets)
std::string Packet::to_string() const {
    std::ostringstream oss;
    oss << "Packet[source_id: " << source_id;
    oss << ", dest_id: " << dest_id;
    oss << ", size(byte): " << size;
    oss << ", flow: " << (flow ? "set" : "null");
    oss << ", sent time: " << sent_time;
    oss << ", flags: " << flags.get_bits();
    oss << "]";

    return oss.str();
}

}  // namespace sim
