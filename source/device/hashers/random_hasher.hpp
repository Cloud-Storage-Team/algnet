#pragma once
#include "i_hasher.hpp"

namespace sim {

class RandomHasher : public IPacketHasher {
public:
    RandomHasher() = default;
    ~RandomHasher() = default;

    std::uint32_t get_hash(Packet packet) final;
};

}