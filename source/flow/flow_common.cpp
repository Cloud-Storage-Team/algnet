#include "flow_common.hpp"

#include "scheduler.hpp"

namespace sim {
FlowCommon::FlowCommon(Id a_id, std::shared_ptr<IHost> a_src,
                       std::shared_ptr<IHost> a_dest, Size a_packet_size,
                       Time a_delay_between_packets,
                       std::uint32_t a_packets_to_send, Time a_delay_threshold)
    : id(a_id),
      src(a_src),
      dest(a_dest),
      packet_size(a_packet_size),
      delay_between_packets(a_delay_between_packets),
      packets_to_send(a_packets_to_send),
      delay_threshold(a_delay_threshold),
      packets_acked(0),
      sent_bytes(0) {}

RoutingPacket FlowCommon::generate_routing_packet() const {
    return RoutingPacket(packet_size, src.lock()->get_id(),
                         dest.lock()->get_id(), sent_bytes,
                         Scheduler::get_instance().get_current_time());
}

std::ostream& operator<<(std::ostream& out, const FlowCommon& flow_common) {
    out << "id: " << flow_common.id;
    return out;
}

}  // namespace sim
