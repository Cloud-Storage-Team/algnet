#include "packet.hpp"
#include <string>

PacketHeader::PacketHeader(std::uint64_t source_id, std::uint64_t destination_id, std::uint64_t sending_time, std::uint32_t packet_index, std::uint64_t rtt, std::uint32_t size):
    source_id(source_id),
    destination_id(destination_id),
    sending_time(sending_time),
    packet_index(packet_index),
    rtt(rtt),
    size(size) {}


std::uint8_t PacketHeader::GetFlag(PacketType bit) const {
    return (flags >> static_cast<uint8_t>(bit)) & 1;
}


void PacketHeader::SetFlag(PacketType bit, std::uint8_t value) {
    if (value == 0) {
        flags = flags & ~((std::uint8_t)1 << static_cast<uint8_t>(bit));
    } else if (value == 1) {
        flags = flags | ((std::uint8_t)1 << static_cast<uint8_t>(bit));
    }
}

bool PacketHeader::operator<(const PacketHeader& other) const {
    return (this->sending_time > other.sending_time) || 
           (this->sending_time == other.sending_time && this->destination_id > other.destination_id);
}