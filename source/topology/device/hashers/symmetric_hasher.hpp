#pragma once
#include "i_hasher.hpp"

namespace sim {

class SymmetricHasher : public IPacketHasher {
public:
    ~SymmetricHasher() = default;

    std::uint32_t get_hash(const Packet& packet) final;
};

}  // namespace sim