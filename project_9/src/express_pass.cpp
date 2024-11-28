#include "express_pass.hpp"

std::uint64_t ExpressPass::getRandomJitter() const {
    // TODO: move to class fields
    std::random_device dev;
    std::mt19937 eng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> jitter(min_jitter,
                                                                    max_jitter);
    return jitter(eng);
}

PacketHeader ExpressPass::GetCredit(std::uint64_t sending_time,
                                    std::uint64_t source_id,
                                    std::uint64_t destination_id,
                                    std::uint64_t size) const {
    // TODO: replace 0 with correct index
    PacketHeader credit =
        PacketHeader(source_id, destination_id, sending_time, 0, size);
    credit.SetFlag(0, 1);
    return credit;
}

PacketHeader ExpressPass::GetHandshakePacket(std::uint64_t sending_time,
                                             std::uint64_t source_id,
                                             std::uint64_t destination_id,
                                             std::uint64_t size) const {
    PacketHeader handshake =
        PacketHeader(source_id, destination_id, sending_time, 0, size);
    handshake.SetFlag(2, 1);
    return handshake;
}

PacketHeader ExpressPass::GetDataPacket(std::uint64_t sending_time,
                                        std::uint64_t source_id,
                                        std::uint64_t destination_id,
                                        std::uint64_t size) const {
    PacketHeader data =
        PacketHeader(source_id, destination_id, sending_time, 0, size);
    return data;
}