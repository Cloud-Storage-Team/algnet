#include "packet.hpp"
#include <string>

PacketHeader::PacketHeader(std::uint64_t source_id, std::uint64_t destination_id, std::uint64_t sending_time, std::uint32_t packet_index, std::uint64_t RTT, std::uint32_t size):
    source_id(source_id),
    destination_id(destination_id),
    sending_time(sending_time),
    packet_index(packet_index),
    RTT(RTT),
    size(size) {}


std::uint8_t PacketHeader::GetFlag(std::uint8_t bit) const {
    return (flags >> bit) & 1;
}


void PacketHeader::SetFlag(std::uint8_t bit, std::uint8_t value) {
    if (value == 0) {
        flags = flags & ~((std::uint8_t)1 << bit);
    } else if (value == 1) {
        flags = flags | ((std::uint8_t)1 << bit);
    }
}

bool PacketHeader::operator<(const PacketHeader& other) const {
    return (this->sending_time > other.sending_time) || 
           (this->sending_time == other.sending_time && this->destination_id > other.destination_id);
}