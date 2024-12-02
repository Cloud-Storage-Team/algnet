#include "Packet.hpp"

Packet::Packet(std::uint64_t sender_id, std::uint64_t sending_time_ns, std::uint64_t delivery_time_ns)
    : sender_id(sender_id), sending_time_ns(sending_time_ns), estimated_delivery_time_ns(delivery_time_ns) { }

// The earlier the estimated_delivery_time, the faster the packet will arrive
bool Packet::operator<(const Packet& other) const {
    return estimated_delivery_time_ns > other.estimated_delivery_time_ns;
}
