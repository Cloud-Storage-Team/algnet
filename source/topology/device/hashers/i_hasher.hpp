#pragma once

#include "network/connection/flow/packet.hpp"

namespace sim {

class IPacketHasher {
public:
    virtual ~IPacketHasher() = default;
    virtual std::uint32_t get_hash(const Packet& packet) = 0;
};

}  // namespace sim
