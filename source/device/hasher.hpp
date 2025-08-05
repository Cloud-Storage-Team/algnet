#pragma once

#include <cstdint>

#include "packet.hpp"

namespace sim {

class IPacketHasher {
public:
    virtual ~IPacketHasher() = default;
    virtual std::uint32_t get_hash(Packet packet, Id deivce_id) = 0;
};

class RandomHasher : public IPacketHasher {
public:
    ~RandomHasher() = default;

    std::uint32_t get_hash(Packet packet, Id device_id) final;
};

class ECMPHasher : public IPacketHasher {
public:
    ~ECMPHasher() = default;

    std::uint32_t get_hash(Packet packet, Id device_id) final;
};

class FLowletHasher : public IPacketHasher {
    ~FLowletHasher() = default;

    std::uint32_t get_hash(Packet packet, Id device_id) final;
};

class SymmetricHasher : public IPacketHasher {
public:
    ~SymmetricHasher() = default;

    std::uint32_t get_hash(Packet packet, Id device_id) final;
};

}  // namespace sim
