#include "packet.hpp"

#include <sstream>

namespace sim {

Packet::Packet(SizeByte a_size, IFlow* a_flow, Route a_route,
               TimeNs a_sent_time, SizeByte a_delivered_at_origin,
               bool a_ecn_capable_transport, bool a_congestion_experienced)
    : flags(0),
      route(std::move(a_route)),
      size(a_size),
      flow(a_flow),
      sent_time(a_sent_time),
      delivered_data_size_at_origin(a_delivered_at_origin),
      ecn_capable_transport(a_ecn_capable_transport),
      congestion_experienced(a_congestion_experienced) {}

bool Packet::operator==(const Packet& packet) const {
    return flow == packet.flow && route == packet.route &&
           size == packet.size && flags == packet.flags;
}

// TODO: think about some ID for packet (currently its impossible to distinguish
// packets)
std::string Packet::to_string() const {
    std::ostringstream oss;
    oss << "Packet[route: " << route;
    oss << ", packet_num: " << packet_num;
    oss << ", size(byte): " << size;
    oss << ", flow: " << (flow ? "set" : "null");
    oss << ", sent time: " << sent_time;
    oss << ", flags: " << flags.get_bits();
    oss << "]";

    return oss.str();
}

}  // namespace sim
