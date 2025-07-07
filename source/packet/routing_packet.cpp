#include "routing_packet.hpp"

namespace sim {
RoutingPacket::RoutingPacket(Size a_size, Id a_source_id, Id a_dest_id,
                             Size a_sent_bytes_at_origin, Time a_send_time)
    : size(a_size),
      source_id(a_source_id),
      dest_id(a_dest_id),
      sent_bytes_at_origin(a_sent_bytes_at_origin),
      sent_time(a_send_time) {}
}  // namespace sim
