#pragma once

#include <cstdint>
#include <ostream>

enum class PacketType:uint8_t {IsCredit = 0, IsAck = 1, IsInitializer = 2};

class PacketHeader {
public:
    std::uint64_t source_id;
    std::uint64_t destination_id;
    // TODO: use it as real sending time insted of time of sending from last network element
    std::uint64_t sending_time;
    std::uint32_t packet_index;
    std::uint32_t size;
    std::uint64_t rtt;

    std::uint8_t flags = 0;

    PacketHeader() = default;
    PacketHeader(std::uint64_t source_id, std::uint64_t destination_id, std::uint64_t sending_time, std::uint32_t packet_index, std::uint64_t rtt, std::uint32_t size);

    std::uint8_t GetFlag(PacketType bit) const;
    void SetFlag(PacketType bit, std::uint8_t value);

    bool operator<(const PacketHeader& other) const;

    friend std::ostream& operator<<(std::ostream& outs, const PacketHeader & packet) {
        outs << "PacketHeader("
             << "Source ID: " << packet.source_id << ", "
             << "Destination ID: " << packet.destination_id << ", "
             << "Sending Time: " << packet.sending_time << ", "
             << "Packet Index: " << packet.packet_index << ", "
             << "rtt: " << packet.rtt << ", "
             << "Size: " << packet.size << ", "
             << "Flags: " << static_cast<int>(packet.flags)
             << ")";
        return outs;
    }
};
