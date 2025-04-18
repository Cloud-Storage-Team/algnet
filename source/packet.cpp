#include "packet.hpp"

namespace sim {
Packet::Packet(PacketType a_type, std::uint32_t a_size, std::weak_ptr<IFlow> a_flow)
    : type(a_type), size(a_size), flow(a_flow) {}

bool Packet::operator==(const Packet& packet) const {
    auto this_flow = flow.lock();
    auto other_flow = packet.flow.lock();
    
    return this_flow == other_flow && 
           size == packet.size && 
           type == packet.type;
}
}  // namespace sim
