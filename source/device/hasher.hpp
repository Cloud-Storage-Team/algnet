#pragma once

#include <cstdint>

#include "packet.hpp"

namespace sim {

class IPacketHasher {
public:
    virtual ~IPacketHasher() = default;
    virtual std::unique_ptr<IPacketHasher> clone() = 0;
    virtual std::uint32_t get_hash(Packet packet) = 0;
};

template <typename Derived>
class ClonebleHasher : public IPacketHasher {
public:
    std::unique_ptr<IPacketHasher> clone() override {
        return std::make_unique<Derived>(static_cast<const Derived&>(*this));
    }
};

class RandomHasher :  public ClonebleHasher<RandomHasher> {
public:
    ~RandomHasher() = default;

    std::uint32_t get_hash(Packet packet) final;
};

class ECMPHasher :  public ClonebleHasher<ECMPHasher> {
public:
    ~ECMPHasher() = default;

    std::uint32_t get_hash(Packet packet) final;
};

class SaltECMPHasher :  public ClonebleHasher<SaltECMPHasher> {
public:
    SaltECMPHasher(Id a_device_id);
    ~SaltECMPHasher() = default;

    std::uint32_t get_hash(Packet packet) final;

private:
    Id m_device_id;
};

class FLowletHasher :  public ClonebleHasher<FLowletHasher> {
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

class SymmetricHasher :  public ClonebleHasher<SymmetricHasher> {
public:
    ~SymmetricHasher() = default;

    std::uint32_t get_hash(Packet packet) final;
};

}  // namespace sim
