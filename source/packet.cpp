#include "packet.hpp"

#include <sstream>

namespace sim {
Packet::Packet(PacketType a_type, Size a_size_byte, IFlow* a_flow,
               Time a_send_time)
    : type(a_type),
      size_byte(a_size_byte),
      flow(a_flow),
      send_time(a_send_time) {}

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
    return flow == packet.flow && size_byte == packet.size_byte &&
           type == packet.type;
}

// TODO: think about some ID for packet (currently its impossible to distinguish
// packets)
std::string Packet::to_string() const {
    std::ostringstream oss;
    oss << "Packet[type: ";

    switch (type) {
        case PacketType::ACK:
            oss << "ACK";
            break;
        case PacketType::DATA:
            oss << "DATA";
            break;
        default:
            oss << "UNKNOWN";
            break;
    }

    oss << ", size(byte): " << size_byte;
    oss << ", flow: " << (flow ? "set" : "null");
    oss << ", send time: " << send_time;
    oss << "]";

    return oss.str();
}

}  // namespace sim
