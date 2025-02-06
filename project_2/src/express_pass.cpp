#include "express_pass.hpp"

std::uint64_t ExpressPass::getRandomJitter() const {
    return static_cast<uint64_t>(default_min_jitter + (rand() % (default_max_jitter + 1)));
}

PacketHeader ExpressPass::GetCredit(std::uint64_t sending_time, std::uint64_t source_id, std::uint64_t destination_id, std::uint32_t index, std::uint64_t size) const {
    // TODO: replace 0 with correct index
    PacketHeader credit = PacketHeader(source_id, destination_id, sending_time, index, sending_time, size);
    credit.SetFlag(PacketType::IsCredit, 1);
    return credit;
}

PacketHeader ExpressPass::GetHandshakePacket(std::uint64_t sending_time, std::uint64_t source_id, std::uint64_t destination_id, std::uint64_t size) const {
    PacketHeader handshake = PacketHeader(source_id, destination_id, sending_time, 0, sending_time, size);
    handshake.SetFlag(PacketType::IsInitializer, 1);
    return handshake;
}

PacketHeader ExpressPass::GetDataPacket(std::uint64_t sending_time, std::uint64_t source_id, std::uint64_t destination_id,  std::uint32_t index, std::uint64_t rtt, std::uint64_t size) const {
    PacketHeader data = PacketHeader(source_id, destination_id, sending_time, index, rtt, size);
    return data;
}