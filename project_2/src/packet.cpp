#include "packet.hpp"
#include <string>

PacketHeader::PacketHeader(std::uint64_t source_id, std::uint64_t destination_id, std::uint64_t sending_time, std::uint32_t packet_index, std::uint32_t size):
    source_id(source_id),
    destination_id(destination_id),
    sending_time(sending_time),
    packet_index(packet_index),
    size(size) {}

std::uint32_t PacketHeader::GetPackageIndex() const {
    return packet_index;
}

std::uint64_t PacketHeader::GetSendingTime() const {
    return sending_time;
}

std::uint64_t PacketHeader::GetSourceID() const {
    return source_id;
}

std::uint64_t PacketHeader::GetDestinationID() const {
    return destination_id;
}

std::uint32_t PacketHeader::GetSize() const {
    return size;
}

std::uint8_t PacketHeader::GetFlag(std::uint8_t bit) const {
    return (flags >> bit) & 1;
}

void PacketHeader::SetSendingTime(std::uint64_t time) {
    sending_time = time;
}

void PacketHeader::SetFlag(std::uint8_t bit, std::uint8_t value) {
    if (value == 0) {
        flags = flags & ~((std::uint8_t)1 << bit);
    } else if (value == 1) {
        flags = flags | ((std::uint8_t)1 << bit);
    }
}

bool PacketHeader::operator<(const PacketHeader& other) const {
    if (this->sending_time > other.sending_time) {
        return true;
    } 
    else if (this->sending_time == other.sending_time && 
             this->destination_id > other.destination_id) {
        return true;
    }
    return false;
}