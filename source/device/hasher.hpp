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
public:
    explicit FLowletHasher(TimeNs a_flowlet_threshold);
    ~FLowletHasher() = default;

    std::uint32_t get_hash(Packet packet, Id device_id) final;

private:
    TimeNs m_flowlet_threshold;

    // Maps flow ids to shift for its packets hash
    std::map<Id, std::uint32_t> m_flow_to_shift;

    // By flow id gives last time some packet from it catched
    std::map<Id, TimeNs> m_last_record;
    
    ECMPHasher m_ecmp_hasher;
};

class SymmetricHasher : public IPacketHasher {
public:
    ~SymmetricHasher() = default;

    std::uint32_t get_hash(Packet packet, Id device_id) final;
};

}  // namespace sim
