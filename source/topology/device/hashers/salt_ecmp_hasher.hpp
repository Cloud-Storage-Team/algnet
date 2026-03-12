#pragma once
#include "i_hasher.hpp"

namespace sim {
class SaltECMPHasher : public IPacketHasher {
public:
    SaltECMPHasher(Id a_device_id);
    ~SaltECMPHasher() = default;

    std::uint32_t get_hash(const Packet& packet) final;

private:
    Id m_device_id;
};

}  // namespace sim