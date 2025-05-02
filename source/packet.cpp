#include "packet.hpp"
#include "device/device.hpp"
#include <sstream>

namespace sim {
Packet::Packet(PacketType a_type, Id a_source_id, Id a_dest_id, Time a_RTT, Size a_size, IFlow* a_flow)

    : type(a_type), source_id(a_source_id), dest_id(a_dest_id), RTT(a_RTT), size(a_size), flow(a_flow) {}

std::shared_ptr<IRoutingDevice> Packet::get_destination() const {
    if (flow == nullptr || flow->get_sender() == nullptr || flow->get_receiver() == nullptr) {
        return nullptr;
    }
    if (dest_id == flow->get_sender()->get_id()) {
        return flow->get_sender();
    } else if (dest_id == flow->get_receiver()->get_id()) {
        return flow->get_receiver();
    } else {
        return nullptr;
    }
};

bool Packet::operator==(const Packet& packet) const {
    return flow == packet.flow && size == packet.size && type == packet.type;
}

// TODO: think about some ID for packet (currently its impossible to distinguish packets)
std::string Packet::to_string() const {
    std::ostringstream oss;
    oss << "Packet[type: ";
    
    switch (type) {
        case PacketType::ACK: oss << "ACK"; break;
        case PacketType::DATA: oss << "DATA"; break;
        case PacketType::CREDIT: oss << "CREDIT"; break;
        case PacketType::CREDIT_REQUEST: oss << "CREQ"; break;
        case PacketType::CREDIT_STOP_P: oss << "CSTOP"; break;
        default: oss << "UNKNOWN"; break;
    }
    
    oss << ", source_id: " << source_id;
    oss << ", dest_id: " << dest_id;
    oss << ", packet_num: " << packet_num;
    oss << ", size: " << size;
    oss << ", flow: " << (flow ? std::to_string(flow->get_id()) : "null");
    oss << "]";
    
    return oss.str();
}

}  // namespace sim
