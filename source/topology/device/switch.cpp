#include "topology/device/switch.hpp"

#include <iostream>

#include "logger/logger.hpp"
#include "scheduler/scheduler.hpp"
#include "utils/defer.hpp"
#include "utils/validation.hpp"

namespace sim {

Switch::Switch(Id a_id, ECN&& a_ecn, std::unique_ptr<IPacketHasher> a_hasher)
    : RoutingModule(a_id, std::move(a_hasher)), m_ecn(std::move(a_ecn)) {}

bool Switch::notify_about_arrival() {
    if (++m_packets_on_inlinks == 1) {
        process();
        return true;
    }
    return false;
};

void Switch::process() {
    static constexpr TimeNs PACKET_PROCESSING_TIME = TimeNs(1);
    std::shared_ptr<ILink> link = next_inlink();

    if (link == nullptr) {
        LOG_WARN("No next inlink");
        return;
    }

    while (!link->has_packet()) {
        link = next_inlink();
    }

    Packet& packet = link->get_packet();
    utils::Defer defer{[link]() { link->pop_packet(); }};

    utils::Defer defer_reschedule{[this]() {
        if (--m_packets_on_inlinks != 0) {
            Scheduler& sched = Scheduler::get_instance();
            sched.add(sched.get_current_time() + PACKET_PROCESSING_TIME,
                      [swtch = shared_from_this()]() { swtch->process(); });
        }
    }};

    std::shared_ptr<ILink> next_link = get_link_to_destination(packet);

    if (next_link == nullptr) {
        LOG_WARN(fmt::format(
            "Switch {}: no link corresponds to destination device for packet "
            "{}",
            get_id(), packet.to_string()));
        return;
    }

    LOG_INFO(fmt::format("Switch {}: processing packet {}", get_id(),
                         packet.to_string()));

    // ECN mark for data packets
    if (packet.ecn_capable_transport) {
        // requests queue size here to consider processing packet
        float ingress_queue_filling = link->get_to_ingress_queue_size() /
                                      link->get_max_to_ingress_queue_size();

        float egress_queue_filling =
            (next_link->get_from_egress_queue_size() + packet.size) /
            next_link->get_max_from_egress_buffer_size();

        if (m_ecn.get_congestion_mark(ingress_queue_filling) ||
            m_ecn.get_congestion_mark(egress_queue_filling)) {
            packet.congestion_experienced = true;
        }
    }

    if (packet.ttl == 0) {
        LOG_ERROR(fmt::format("Packet ttl expired on switch {}; packet {} lost",
                              get_id(), packet.to_string()));
        return;
    }
    packet.ttl--;
    packet.path_hash ^= std::hash<Id>{}(get_id());

    // TODO: increase total_processing_time correctly
    next_link->schedule_arrival(packet);

    return;
}

}  // namespace sim
