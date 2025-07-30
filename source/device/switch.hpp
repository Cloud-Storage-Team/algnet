#pragma once

#include "device/interfaces/i_switch.hpp"
#include "device/routing_module.hpp"
#include "device/scheduling_module.hpp"
#include "ecn.hpp"
#include "event/process.hpp"

namespace sim {

class Switch : public ISwitch,
               public RoutingModule,
               public std::enable_shared_from_this<Switch> {
public:
    Switch(Id a_id, ECN&& a_ecn = ECN(1.0, 1.0, 0.0));
    ~Switch() = default;

    bool notify_about_arrival(TimeNs arrival_time) final;

    // Process a packet by moving it from ingress to egress
    // and schedule next process event after a delay.
    // Packets are taken from ingress buffers on a round-robin basis.
    // The iterator over ingress buffers is stored in m_next_link.
    TimeNs process() final;

private:
    SchedulingModule<ISwitch, Process> m_process_scheduler;
    ECN m_ecn;
};

}  // namespace sim
