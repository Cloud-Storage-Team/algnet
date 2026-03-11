#include "ecmp_hasher.hpp"

namespace sim {

std::uint32_t ECMPHasher::get_hash(const Packet& packet) {
    std::hash<std::string> hasher;
    std::string header_str =
        fmt::format("{} {} {} {}", packet.sender_id, packet.receiver_id,
                    packet.sender_port, packet.receriver_port);
    return static_cast<uint32_t>(hasher(header_str));
}

}  // namespace sim
