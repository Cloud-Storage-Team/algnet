#include "packet.hpp"
#include <sstream>

namespace sim {
Packet::Packet(PacketType a_type, Size a_size, std::weak_ptr<IFlow> a_flow)
    : type(a_type), size(a_size), flow(a_flow) {}

std::shared_ptr<IRoutingDevice> Packet::get_destination() const {
    if (!flow.lock()) {
        return nullptr;
    }
    switch (type) {
        case ACK:
            return flow.lock()->get_sender();
        case DATA:
            return flow.lock()->get_receiver();
        default:
            return nullptr;
    }
};

bool Packet::operator==(const Packet& packet) const {
    auto this_flow = flow.lock();
    auto other_flow = packet.flow.lock();
    
    return this_flow == other_flow && 
           size == packet.size && 
           type == packet.type;
}

// TODO: think about some ID for packet (currently its impossible to distinguish packets)
std::string Packet::to_string() const {
    std::ostringstream oss;
    oss << "Packet[type: ";
    
    switch (type) {
        case PacketType::ACK: oss << "ACK"; break;
        case PacketType::DATA: oss << "DATA"; break;
        default: oss << "UNKNOWN"; break;
    }
    
    oss << ", size: " << size;
    oss << ", flow: " << (flow.lock() ? "set" : "null");
    oss << "]";
    
    return oss.str();
}

}  // namespace sim
