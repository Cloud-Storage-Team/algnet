#pragma once
#include "i_hasher.hpp"

namespace sim {

class ECMPHasher : public IPacketHasher {
public:
    ECMPHasher() = default;
    ~ECMPHasher() = default;

    std::uint32_t get_hash(Packet packet) final;
};
}  // namespace sim