#include "salt_ecmp_hasher.hpp"

namespace sim {

SaltECMPHasher::SaltECMPHasher(Id a_device_id)
    : m_device_id(std::move(a_device_id)) {}

std::uint32_t SaltECMPHasher::get_hash(const Packet& packet) {
    std::hash<std::string> hasher;
    std::string header_str =
        fmt::format("{} {} {} {} {}", packet.sender_id, packet.sender_port,
                    packet.receiver_id, packet.receriver_port, m_device_id);
    return static_cast<uint32_t>(hasher(header_str));
}

}  // namespace sim
