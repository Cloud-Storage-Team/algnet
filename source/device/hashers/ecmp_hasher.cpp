#include "ecmp_hasher.hpp"

namespace sim {

std::uint32_t ECMPHasher::get_hash(const Packet& packet) {
    std::string flow_id = packet.flow_id;

    std::hash<std::string> hasher;
    std::string header_str =
        fmt::format("{} {} {}", flow_id, packet.source_id, packet.dest_id);
    return static_cast<uint32_t>(hasher(header_str));
}

}  // namespace sim