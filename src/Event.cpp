#include "Event.hpp"

#include <utility>

Event::Event(std::shared_ptr<Packet> packet, std::uint32_t flow_id, std::uint32_t receiver_id, std::uint64_t delivery_time_ns):
    delivery_time_ns(delivery_time_ns),
    flow_id(flow_id),
    receiver_id(receiver_id),
    packet(std::move(packet)) { }

bool Event::operator<(const Event& other) const {
    return delivery_time_ns > other.delivery_time_ns;
}