#include "packet.hpp"

namespace sim {
Packet::Packet(PacketType a_type, std::uint32_t a_size, IFlow* a_flow)
    : type(a_type), size(a_size), flow(a_flow) {}

bool Packet::operator==(const Packet& packet) const {
    return flow == packet.flow && size == packet.size && type == packet.type;
}
}  // namespace sim
