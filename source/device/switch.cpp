#include "device/switch.hpp"

#include <iostream>

#include "device/routing_module.hpp"
#include "logger/logger.hpp"
#include "utils/validation.hpp"

namespace sim {

Switch::Switch(Id a_id, bool a_ecn_capable_transport, double a_ecn_threshold)
    : m_ecn_capable_transport(a_ecn_capable_transport),
      m_ecn_threshold(a_ecn_threshold),
      m_router(std::make_unique<RoutingModule>(a_id)) {}

bool Switch::add_inlink(std::shared_ptr<ILink> link) {
    if (!is_valid_link(link)) [[unlikely]] {
        return false;
    }
    return m_router->add_inlink(link);
}

bool Switch::add_outlink(std::shared_ptr<ILink> link) {
    if (!is_valid_link(link)) [[unlikely]] {
        return false;
    }
    return m_router->add_outlink(link);
}

bool Switch::update_routing_table(Id dest_id, std::shared_ptr<ILink> link,
                                  size_t paths_count) {
    if (!is_valid_link(link)) [[unlikely]] {
        return false;
    }
    return m_router->update_routing_table(dest_id, link, paths_count);
}

std::shared_ptr<ILink> Switch::next_inlink() { return m_router->next_inlink(); }

std::shared_ptr<ILink> Switch::get_link_to_destination(Packet packet) const {
    return m_router->get_link_to_destination(packet);
}

bool Switch::notify_about_arrival(Time arrival_time) {
    return m_process_scheduler.notify_about_arriving(arrival_time,
                                                     weak_from_this());
};

DeviceType Switch::get_type() const { return DeviceType::SWITCH; }

Time Switch::process() {
    Time total_processing_time = 1;
    std::shared_ptr<ILink> link = next_inlink();

    if (link == nullptr) [[unlikely]] {
        LOG_WARN("No next inlink");
        return total_processing_time;
    }

    std::optional<Packet> optional_packet = link->get_packet();
    if (!optional_packet.has_value()) {
        LOG_WARN("No packet in link");
        return total_processing_time;
    }
    Packet packet = optional_packet.value();
    if (packet.flow == nullptr) [[unlikely]] {
        LOG_WARN("No flow in packet");
        return total_processing_time;
    }

    std::shared_ptr<ILink> next_link = get_link_to_destination(packet);

    if (next_link == nullptr) [[unlikely]] {
        LOG_WARN("No link corresponds to destination device");
        return total_processing_time;
    }

    // TODO: add some switch ID for easier packet path tracing
    LOG_INFO("Processing packet from link on switch. Packet: " +
             packet.to_string());

    // ECN mark for data packets
    if (m_ecn_capable_transport && packet.type == DATA &&
        packet.ecn_capable_transport &&
        static_cast<double>(next_link->get_from_egress_queue_size()) >=
            m_ecn_threshold *
                static_cast<double>(
                    next_link->get_max_from_egress_buffer_size())) {
        packet.congestion_experienced = true;
    }
    // TODO: increase total_processing_time correctly
    next_link->schedule_arrival(packet);

    if (m_process_scheduler.notify_about_finish(
            Scheduler::get_instance().get_current_time() +
            total_processing_time)) {
        return 0;
    }

    return total_processing_time;
}

std::set<std::shared_ptr<ILink>> Switch::get_outlinks() {
    return m_router->get_outlinks();
}

Id Switch::get_id() const { return m_router->get_id(); }

}  // namespace sim
