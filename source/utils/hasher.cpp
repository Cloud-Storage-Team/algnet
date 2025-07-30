
#include "utils/hasher.hpp"

#include <spdlog/fmt/fmt.h>

namespace sim {

std::uint32_t RandomHasher::get_hash(Packet) const {
    return static_cast<std::uint32_t>(std::rand());
};

std::uint32_t BaseHasher::get_hash(Packet packet) const {
    std::hash<std::string> hasher;
    return static_cast<uint32_t>(hasher(packet.route.to_string()));
};

std::uint32_t SymmetricHasher::get_hash(Packet packet) const {
    std::hash<std::string> hasher;

    return static_cast<uint32_t>(hasher(packet.route.source.to_string()) ^
                                 hasher(packet.route.dest.to_string()));
};

}  // namespace sim
