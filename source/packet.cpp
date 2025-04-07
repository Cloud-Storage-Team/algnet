#include "packet.hpp"
#include <sstream>

namespace sim {
Packet::Packet(PacketType a_type, std::uint32_t a_size, IFlow* a_flow)
    : type(a_type), size(a_size), flow(a_flow) {}

std::shared_ptr<IRoutingDevice> Packet::get_destination() const {
    if (flow == nullptr) {
        return nullptr;
    }
    switch (type)
    {
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

std::string Packet::to_string() const {
    std::ostringstream oss;
    oss << "Packet[type: ";
    
    switch (type) {
        case PacketType::ACK: oss << "ACK"; break;
        case PacketType::DATA: oss << "DATA"; break;
        default: oss << "UNKNOWN"; break;
    }
    
    oss << ", size: " << size;
    oss << ", flow: " << (flow ? "set" : "null");
    oss << "]";
    
    return oss.str();
}
}  // namespace sim
