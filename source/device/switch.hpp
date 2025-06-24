#pragma once

#include "device/interfaces/i_switch.hpp"
#include "device/scheduling_module.hpp"
#include "event.hpp"

namespace sim {

class Switch : public ISwitch, public std::enable_shared_from_this<Switch> {
public:
    Switch(Id a_id, double a_ecn_threshold_percent = 1.0);
    ~Switch() = default;

    bool add_inlink(std::shared_ptr<ILink> link) final;
    bool add_outlink(std::shared_ptr<ILink> link) final;
    bool update_routing_table(Id dest_id, std::shared_ptr<ILink> link,
                              size_t paths_count = 1) final;
    std::shared_ptr<ILink> next_inlink() final;
    std::shared_ptr<ILink> get_link_to_destination(Packet packet) const final;
    std::set<std::shared_ptr<ILink>> get_outlinks() final;
    bool notify_about_arrival(Time arrival_time) final;

    DeviceType get_type() const final;
    // Process a packet by moving it from ingress to egress
    // and schedule next process event after a delay.
    // Packets are taken from ingress buffers on a round-robin basis.
    // The iterator over ingress buffers is stored in m_next_link.
    Time process() final;

    Id get_id() const final;

private:
    double m_ecn_threshold;
    std::unique_ptr<IRoutingDevice> m_router;
    SchedulingModule<ISwitch, Process> m_process_scheduler;
};

}  // namespace sim
