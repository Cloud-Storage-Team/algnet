
#include "hasher.hpp"

#include <spdlog/fmt/fmt.h>

namespace sim {

std::uint32_t RandomHasher::get_hash(Packet, Id) {
    return static_cast<std::uint32_t>(std::rand());
};

std::uint32_t ECMPHasher::get_hash(Packet packet, Id) {
    std::string flow_id_str =
        ((packet.flow == nullptr) ? "0" : packet.flow->get_id());

    std::hash<std::string> hasher;
    std::string header_str =
        fmt::format("{} {} {}", flow_id_str, packet.source_id, packet.dest_id);
    return static_cast<uint32_t>(hasher(header_str));
};

std::uint32_t SymmetricHasher::get_hash(Packet packet, Id) {
    std::hash<std::string> hasher;

    std::string combined_id_str =
        std::to_string(hasher(packet.source_id) ^ hasher(packet.dest_id));
    std::string flow_id_str =
        ((packet.flow == nullptr) ? "0" : packet.flow->get_id());

    std::string header_str = fmt::format("{} {}", flow_id_str, combined_id_str);
    return static_cast<uint32_t>(hasher(header_str));
};

}  // namespace sim
