#pragma once

#include <cstdint>

#include "packet.hpp"

namespace sim {

class IPacketHasher {
public:
    virtual ~IPacketHasher() = default;
    virtual std::uint32_t get_hash(Packet packet, Id deivce_id) const = 0;
};

class RandomHasher : public IPacketHasher {
public:
    ~RandomHasher() = default;

    std::uint32_t get_hash(Packet packet, Id device_id) const final;
};

class ECMPHasher : public IPacketHasher {
public:
    ~ECMPHasher() = default;

    std::uint32_t get_hash(Packet packet, Id device_id) const final;
};

class SymmetricHasher : public IPacketHasher {
public:
    ~SymmetricHasher() = default;

    std::uint32_t get_hash(Packet packet, Id device_id) const final;
};

}  // namespace sim
