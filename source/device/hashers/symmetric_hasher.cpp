
#include "symmetric_hasher.hpp"

namespace sim {
std::uint32_t SymmetricHasher::get_hash(const Packet& packet) {
    std::hash<std::string> hasher;

    std::string combined_id_str =
        std::to_string(hasher(packet.source_id) ^ hasher(packet.dest_id));
    std::string flow_id = packet.flow_id;

    std::string header_str = fmt::format("{} {}", flow_id, combined_id_str);
    return static_cast<uint32_t>(hasher(header_str));
}

}  // namespace sim
