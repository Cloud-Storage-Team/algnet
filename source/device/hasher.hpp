#pragma once

#include <cstdint>

#include "packet.hpp"

namespace sim {

class IPacketHasher {
public:
    virtual ~IPacketHasher() = default;
    virtual std::uint32_t get_hash(Packet packet) = 0;
};

class RandomHasher : public IPacketHasher {
public:
    ~RandomHasher() = default;

    std::uint32_t get_hash(Packet packet) final;
};

class ECMPHasher : public IPacketHasher {
public:
    ~ECMPHasher() = default;

    std::uint32_t get_hash(Packet packet) final;
};

class SaltECMPHasher : public IPacketHasher {
public:
    SaltECMPHasher(Id a_device_id);
    ~SaltECMPHasher() = default;

    std::uint32_t get_hash(Packet packet) final;

private:
    Id m_device_id;
};

class FLowletHasher : public IPacketHasher {
public:
    explicit FLowletHasher(TimeNs a_flowlet_threshold);
    ~FLowletHasher() = default;

    std::uint32_t get_hash(Packet packet) final;

private:
    TimeNs m_flowlet_threshold;

    // Maps flow ids to pair (last_time, shift)
    // where last_time is the last time packet from given flow was catched
    // and shift is a integer that should be addeded to ECMP hash for packets
    // from this flow
    std::map<Id, std::pair<TimeNs, std::uint32_t> > m_flow_table;

    ECMPHasher m_ecmp_hasher;
};

class SymmetricHasher : public IPacketHasher {
public:
    ~SymmetricHasher() = default;

    std::uint32_t get_hash(Packet packet) final;
};

}  // namespace sim
