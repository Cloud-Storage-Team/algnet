#include "packet.hpp"

#include <sstream>

namespace sim {

Packet::Packet(SizeByte a_size_byte, IFlow* a_flow, Id a_source_id,
               Id a_dest_id, TimeNs a_sent_time,
               SizeByte a_sent_bytes_at_origin, bool a_ecn_capable_transport,
               bool a_congestion_experienced)
    : flags(0),
      source_id(a_source_id),
      dest_id(a_dest_id),
      size_byte(a_size_byte),
      flow(a_flow),
      sent_time(a_sent_time),
      sent_bytes_at_origin(a_sent_bytes_at_origin),
      ecn_capable_transport(a_ecn_capable_transport),
      congestion_experienced(a_congestion_experienced) {}

bool Packet::operator==(const Packet& packet) const {
    return flow == packet.flow && source_id == packet.source_id &&
           dest_id == packet.dest_id && size_byte == packet.size_byte &&
           flags == packet.flags;
}

// TODO: think about some ID for packet (currently its impossible to distinguish
// packets)
std::string Packet::to_string() const {
    std::ostringstream oss;
    oss << "Packet[source_id: " << source_id;
    oss << ", dest_id: " << dest_id;
    oss << ", packet_num: " << packet_num;
    oss << ", size(byte): " << size_byte;
    oss << ", flow: " << (flow ? "set" : "null");
    oss << ", sent time: " << sent_time;
    oss << ", flags: " << flags.get_bits();
    oss << "]";

    return oss.str();
}

}  // namespace sim
