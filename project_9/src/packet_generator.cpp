#include "packet_generator.hpp"

PacketHeader PacketGenerator::GetAckPacket(std::uint64_t sending_time,
                                           std::uint64_t source_id,
                                           std::uint64_t destination_id,
                                           std::uint64_t size) const {
    PacketHeader ack =
        PacketHeader(source_id, destination_id, sending_time, 0, size);
    ack.SetFlag(0, 1);
    return ack;
}

PacketHeader PacketGenerator::GetDataPacket(std::uint64_t sending_time,
                                            std::uint64_t source_id,
                                            std::uint64_t destination_id,
                                            std::uint64_t size) const {
    PacketHeader data =
        PacketHeader(source_id, destination_id, sending_time, 0, size);
    return data;
}
