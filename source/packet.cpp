#include "packet.hpp"

#include <sstream>
#include "device/interfaces/i_processing_device.hpp"
#include "flow/interfaces/i_flow.hpp"
#include <string>

namespace sim {

Packet::Packet(PacketType a_type, Size a_size_byte, IFlow* a_flow, Id a_source_id, Id a_dest_id, Time a_RTT, Time a_send_time)
    : type(a_type),
      source_id(a_source_id),
      dest_id(a_dest_id),
      RTT(a_RTT),
      size_byte(a_size_byte),
      flow(a_flow),
      send_time(a_send_time) {}

bool Packet::operator==(const Packet& packet) const {
    return flow == packet.flow && 
           source_id == packet.source_id && 
           dest_id == packet.dest_id &&
           size_byte == packet.size_byte && 
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
    
    oss << ", source_id: " << source_id;
    oss << ", dest_id: " << dest_id;
    oss << ", packet_num: " << packet_num;
    oss << ", size(byte): " << size_byte;
    oss << ", flow: " << (flow ? "set" : "null");
    oss << ", send time: " << send_time;
    oss << "]";

    return oss.str();
}

}  // namespace sim
