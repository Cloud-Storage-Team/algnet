#include <cstdint>

#include <spdlog/fmt/fmt.h>

#include "hasher.hpp"

namespace sim {

std::uint32_t RandomHasher::get_hash(Packet) const {
    return static_cast<std::uint32_t>(std::rand());
};

std::uint32_t BaseHasher::get_hash(Packet packet) const {
    std::string flow_id_str = ((packet.flow == nullptr) ? "0" : std::to_string(packet.flow->get_id()));

    std::hash<std::string> hasher;
    std::string header_str = fmt::format("{} {} {}", flow_id_str, packet.source_id, packet.dest_id);
    return static_cast<uint32_t>(hasher(header_str));
};

std::uint32_t SymmetricHasher::get_hash(Packet packet) const {
    std::string combined_id_str = std::to_string(packet.source_id ^ packet.dest_id);
    std::string flow_id_str = ((packet.flow == nullptr) ? "0" : std::to_string(packet.flow->get_id()));
    
    std::hash<std::string> hasher;
    std::string header_str = fmt::format("{} {}", flow_id_str, combined_id_str);
    return static_cast<uint32_t>(hasher(header_str));
};

} // namespace sim
