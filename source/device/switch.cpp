#include "device/switch.hpp"

#include <iostream>
#include <spdlog/fmt/fmt.h>

#include "device/routing_module.hpp"
#include "logger/logger.hpp"
#include "utils/validation.hpp"

namespace sim {

Switch::Switch(Id a_id, ECN&& a_ecn)
    : m_router(std::make_unique<RoutingModule>(a_id)),
      m_ecn(std::move(a_ecn)) {}

bool Switch::add_inlink(std::shared_ptr<ILink> link) {
    if (!is_valid_link(link)) {
        return false;
    }
    return m_router->add_inlink(link);
}

bool Switch::add_outlink(std::shared_ptr<ILink> link) {
    if (!is_valid_link(link)) {
        return false;
    }
    return m_router->add_outlink(link);
}

bool Switch::update_routing_table(Id dest_id, std::shared_ptr<ILink> link,
                                  size_t paths_count) {
    if (!is_valid_link(link)) {
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

    if (link == nullptr) {
        // TODO: think about calling this log once
        LOG_ERROR("No available inlinks for device");
        return total_processing_time;
    }

    // requests queue size here to consider processing packet
    float ingress_queue_filling = link->get_to_ingress_queue_size() /
                               (float)link->get_max_to_ingress_queue_size();
    std::optional<Packet> optional_packet = link->get_packet();
    if (!optional_packet.has_value()) {
        // This log brings approximately 0 information
        // TODO: think about removing or lowering level
        // LOG_WARN("No available packets from inlink for device");
        return total_processing_time;
    }
    Packet packet = optional_packet.value();
    if (packet.flow == nullptr) {
        // Do we really need to log it here?
        // TODO: think about moving or removing
        // LOG_ERROR("Packet flow does not exist");
        return total_processing_time;
    }

    std::shared_ptr<ILink> next_link = get_link_to_destination(packet);

    if (next_link == nullptr) {
        // TODO: think about and increasing log level
        LOG_WARN(fmt::format("No link corresponds to destination device. Current switch id: {}, destination device id: {}",
                 get_id(), packet.dest_id));
        return total_processing_time;
    }

    LOG_INFO(fmt::format("Processing packet from link(id: {}) on switch(id: {}). Packet: {}",
         next_link->get_id(), get_id(), packet.to_string()));

    // ECN mark for data packets
    if (packet.ecn_capable_transport) {
        float egress_queue_filling =
            next_link->get_from_egress_queue_size() /
            (float)next_link->get_max_from_egress_buffer_size();
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
        return 0;
    }

    return total_processing_time;
}

std::set<std::shared_ptr<ILink>> Switch::get_outlinks() {
    return m_router->get_outlinks();
}

Id Switch::get_id() const { return m_router->get_id(); }

}  // namespace sim
