#include "device/switch.hpp"

#include <iostream>

#include "logger/logger.hpp"
#include "utils/validation.hpp"

namespace sim {

Switch::Switch(Id a_id, ECN&& a_ecn)
    : RoutingModule(a_id), m_ecn(std::move(a_ecn)) {}

bool Switch::notify_about_arrival(TimeNs arrival_time) {
    return m_process_scheduler.notify_about_arriving(arrival_time,
                                                     weak_from_this());
};

DeviceType Switch::get_type() const { return DeviceType::SWITCH; }

TimeNs Switch::process() {
    TimeNs total_processing_time = TimeNs(1);
    std::shared_ptr<ILink> link = next_inlink();

    if (link == nullptr) {
        LOG_WARN("No next inlink");
        return total_processing_time;
    }

    // requests queue size here to consider processing packet
    float ingress_queue_filling = link->get_to_ingress_queue_size() /
                                  link->get_max_to_ingress_queue_size();
    std::optional<Packet> optional_packet = link->get_packet();
    if (!optional_packet.has_value()) {
        LOG_WARN("No packet in link");
        return total_processing_time;
    }
    Packet packet = optional_packet.value();
    if (packet.flow == nullptr) {
        LOG_WARN("No flow in packet");
        return total_processing_time;
    }

    std::shared_ptr<ILink> next_link = get_link_to_destination(packet);

    if (next_link == nullptr) {
        LOG_WARN("No link corresponds to destination device");
        return total_processing_time;
    }

    // TODO: add some switch ID for easier packet path tracing
    LOG_INFO("Processing packet from link on switch. Packet: " +
             packet.to_string());

    // ECN mark for data packets
    if (packet.ecn_capable_transport) {
        float egress_queue_filling =
            next_link->get_from_egress_queue_size() /
            next_link->get_max_from_egress_buffer_size();
        if (m_ecn.get_congestion_mark(ingress_queue_filling) ||
            m_ecn.get_congestion_mark(egress_queue_filling)) {
            packet.congestion_experienced = true;
        }
    }
    // TODO: increase total_processing_time correctly
    next_link->schedule_arrival(packet);

    if (m_process_scheduler.notify_about_finish(
            Scheduler::get_instance().get_current_time() +
            total_processing_time)) {
        return TimeNs(0);
    }

    return total_processing_time;
}

}  // namespace sim
