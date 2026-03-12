
#include "symmetric_hasher.hpp"

namespace sim {
std::uint32_t SymmetricHasher::get_hash(const Packet& packet) {
    std::hash<std::string> hasher;

    return hasher(packet.sender_id) ^ hasher(packet.receiver_id) ^
           packet.sender_port ^ packet.receiver_port;
}

}  // namespace sim
