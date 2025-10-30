#include "packet.hpp"

#include <sstream>

namespace sim {

Packet::Packet(SizeByte a_size, IFlow* a_flow, Id a_source_id, Id a_dest_id,
               TimeNs a_generated_time, TimeNs a_sent_time,
               SizeByte a_delivered_data_size_at_origin,
               bool a_ecn_capable_transport, bool a_congestion_experienced)
    : flags(),
      source_id(a_source_id),
      dest_id(a_dest_id),
      size(a_size),
      flow(a_flow),
      generated_time(a_generated_time),
      sent_time(a_sent_time),
      delivered_data_size_at_origin(a_delivered_data_size_at_origin),
      ecn_capable_transport(a_ecn_capable_transport),
      congestion_experienced(a_congestion_experienced) {}

bool Packet::operator==(const Packet& packet) const {
    return flow == packet.flow && source_id == packet.source_id &&
           dest_id == packet.dest_id && size == packet.size &&
           flags.get_bits() == packet.flags.get_bits();
}

// TODO: think about some ID for packet (currently its impossible to distinguish
// packets)
std::string Packet::to_string() const {
    std::ostringstream oss;
    oss << "Packet[source_id: " << source_id;
    oss << ", dest_id: " << dest_id;
    oss << ", packet_num: " << packet_num;
    oss << ", size(byte): " << size;
    oss << ", flow: " << (flow ? "set" : "null");
    oss << ", generated time: " << generated_time;
    oss << ", sent time: " << sent_time;
    oss << ", TTL: " << ttl;
    oss << ", flags: " << flags.get_bits().get_bits();
    oss << "]";

    return oss.str();
}

}  // namespace sim
