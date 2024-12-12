#pragma once

#include <cstdint>
#include <ostream>

/**
 * @brief Network packet abstraction
 * 
 */
class PacketHeader {
public:
    std::uint64_t source_id;
    std::uint64_t destination_id;
    // TODO: use it as real sending time (passing first link maybe) in checks that
    std::uint64_t sending_time;
    std::uint32_t packet_index;
    std::uint32_t size;

    // 0 bit - is credit packet
    // 2 bit - initializing connection or not
    std::uint8_t flags = 0;

    PacketHeader() = default;
    /**
     * @brief Construct a new Packet Header object
     * 
     * @param source_id packet source
     * @param destination_id packet destination
     * @param sending_time time of sending packet 
     * @param packet_index unique packet identifier (within flow)
     * @param size packet size
     */
    PacketHeader(std::uint64_t source_id, std::uint64_t destination_id, std::uint64_t sending_time, std::uint32_t packet_index, std::uint32_t size);

    std::uint8_t GetFlag(std::uint8_t bit) const;
    void SetFlag(std::uint8_t bit, std::uint8_t value);

    bool operator<(const PacketHeader& other) const;

    friend std::ostream& operator<<(std::ostream& outs, const PacketHeader & packet) {
        outs << "PacketHeader("
             << "Source ID: " << packet.source_id << ", "
             << "Destination ID: " << packet.destination_id << ", "
             << "Sending Time: " << packet.sending_time << ", "
             << "Packet Index: " << packet.packet_index << ", "
             << "Size: " << packet.size << ", "
             << "Flags: " << static_cast<int>(packet.flags)
             << ")";
        return outs;
    }
};
