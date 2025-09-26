#include "salt_ecmp_hasher.hpp"

#include "get_flow_id.hpp"

namespace sim {

SaltECMPHasher::SaltECMPHasher(Id a_device_id)
    : m_device_id(std::move(a_device_id)) {}

std::uint32_t SaltECMPHasher::get_hash(Packet packet) {
    std::string flow_id = get_flow_id(packet.flow);

    std::hash<std::string> hasher;
    std::string header_str = fmt::format(
        "{} {} {} {}", flow_id, packet.source_id, packet.dest_id, m_device_id);
    return static_cast<uint32_t>(hasher(header_str));
}

}  // namespace sim