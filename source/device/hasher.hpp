#pragma once

#include <cstdint>

#include "packet.hpp"

namespace sim {

class IPacketHasher {
public:
    virtual ~IPacketHasher() = default;
    virtual std::uint32_t get_hash(Packet packet) const = 0;
};

class RandomHasher : public IPacketHasher {
public:
    ~RandomHasher() = default;

    std::uint32_t get_hash(Packet packet) const final;
};

class BaseHasher : public IPacketHasher {
public:
    ~BaseHasher() = default;

    std::uint32_t get_hash(Packet packet) const final;
};

class SymmetricHasher : public IPacketHasher {
public:
    ~SymmetricHasher() = default;

    std::uint32_t get_hash(Packet packet) const final;
};

} // namespace sim
