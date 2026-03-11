
#include "symmetric_hasher.hpp"

namespace sim {
std::uint32_t SymmetricHasher::get_hash(const Packet& packet) {
    std::hash<std::string> hasher;

    std::string combined_id_str =
        std::to_string(hasher(packet.source_id) ^ hasher(packet.dest_id));

    // TODO: fix it (add four-tuple)

    std::string header_str = combined_id_str;
    return static_cast<uint32_t>(hasher(header_str));
}

}  // namespace sim
