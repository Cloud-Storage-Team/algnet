#pragma once

#include <queue>
#include <vector>

#include "packet.hpp"
#include "utils.hpp"

class NetworkSwitch : public RoutingNetworkElement {
  public:
    virtual std::uint64_t
    SendPackets(std::uint64_t current_time_ns,
                PriorityQueueWrapper &packets_wrapped) = 0;
    virtual void
    ReceivePacket(std::uint64_t current_time_ns, PacketHeader &packet,
                  PriorityQueueWrapper &packets_wrapped) override = 0;
    virtual ~NetworkSwitch() {
    }
};
