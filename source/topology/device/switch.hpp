#pragma once

#include "ecn.hpp"
#include "routing_module/routing_module.hpp"
#include "topology/device/interfaces/i_switch.hpp"

namespace sim {

class Switch : public ISwitch,
               public RoutingModule,
               public std::enable_shared_from_this<Switch> {
public:
    Switch(Id a_id, ECN&& a_ecn = ECN(1.0, 1.0, 0.0),
           std::unique_ptr<IPacketHasher> a_packet_hasher = nullptr);
    ~Switch() = default;

    bool notify_about_arrival() final;

private:
    // Process a packet by moving it from ingress to egress
    // and schedule next process event after a delay.
    // Packets are taken from ingress buffers on a round-robin basis.
    // The iterator over ingress buffers is stored in m_next_link.
    void process();

    ECN m_ecn;
    std::size_t m_packets_on_inlinks = 0;
};

}  // namespace sim
