
#include "symmetric_hasher.hpp"

namespace sim {
std::uint32_t SymmetricHasher::get_hash(PacketPtr packet) {
    return packet->sender_id.hash ^ packet->receiver_id.hash ^
           packet->sender_port ^ packet->receiver_port;
}

}  // namespace sim
