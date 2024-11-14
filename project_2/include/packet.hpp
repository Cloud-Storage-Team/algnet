#pragma once

#include <cstdint>
#include <ostream>

// Network packet abstraction
class PacketHeader {
private:
    std::uint64_t source_id;
    std::uint64_t destination_id;
    std::uint64_t sending_time;
    std::uint32_t packet_index;
    std::uint32_t size;

    // 0 bit - is credit packet
    // 2 bit - initializing connection or not
    std::uint8_t flags = 0;
public:
    PacketHeader() = default;
    PacketHeader(std::uint64_t source_id, std::uint64_t destination_id, std::uint64_t sending_time, std::uint32_t packet_index, std::uint32_t size);

    std::uint32_t GetPackageIndex() const;
    std::uint64_t GetDestinationID() const;
    std::uint64_t GetSendingTime() const;
    std::uint64_t GetSourceID() const;
    std::uint32_t GetSize() const;
    std::uint8_t GetFlag(std::uint8_t bit) const;

    void SetSendingTime(std::uint64_t time);
    void SetFlag(std::uint8_t bit, std::uint8_t value);

    bool operator<(const PacketHeader& other) const;

    friend std::ostream& operator<<(std::ostream& outs, const PacketHeader & packet) {
        outs << "PacketHeader("
             << "Source ID: " << packet.source_id << ", "
             << "Destination ID: " << packet.destination_id << ", "
             << "Sending Time: " << packet.sending_time << ", "
            //  << "Packet Index: " << packet.packet_index << ", "
             << "Size: " << packet.size << ", "
            //  << "Flags: " << static_cast<int>(packet.flags)
             << ")";
        return outs;
    }
};
