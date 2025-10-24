#pragma once
#include "types.hpp"

namespace sim {

using PacketReordering = uint64_t;

// Metrics that shows degree of packet reordering
class IPacketReordering {
public:
    virtual ~IPacketReordering() = default;

    // Callback that should be triggered when packet
    // with number packet_num arrives
    virtual void add_record(PacketNum packet_num) = 0;
    virtual PacketReordering value() const = 0;
};

}  // namespace sim