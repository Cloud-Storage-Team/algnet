#pragma once

#include <cstdint>

#include "packet.hpp"

namespace sim {

class IHasher {
public:
    virtual ~IHasher() = default;
    virtual std::uint32_t get_hash(Packet packet) const = 0;
};

class RandomHasher : public IHasher {
public:
    ~RandomHasher() = default;

    std::uint32_t get_hash(Packet packet) const final;
};

class BaseHasher : public IHasher {
public:
    ~BaseHasher() = default;

    std::uint32_t get_hash(Packet packet) const final;
};

class SymmetricHasher : public IHasher {
public:
    ~SymmetricHasher() = default;

    std::uint32_t get_hash(Packet packet) const final;
};

} // namespace sim
