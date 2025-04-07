#include "packet.hpp"

namespace sim {
Packet::Packet(PacketType a_type, std::uint32_t a_size, IFlow* a_flow)
    : type(a_type), size(a_size), flow(a_flow) {}

std::shared_ptr<IRoutingDevice> Packet::get_destination() const {
    if (flow == nullptr) {
        return nullptr;
    }
    switch (type) {
        case ACK:
            return flow->get_sender();
        case DATA:
            return flow->get_receiver();
        default:
            return nullptr;
    }
};

bool Packet::operator==(const Packet& packet) const {
    return flow == packet.flow && size == packet.size && type == packet.type;
}
}  // namespace sim
