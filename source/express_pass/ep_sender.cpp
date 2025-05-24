#include "express_pass/ep_sender.hpp"

#include <spdlog/fmt/fmt.h>

#include <memory>

// #include "event.hpp"
// #include "link.hpp"
// #include "logger/logger.hpp"
#include "scheduler.hpp"
#include "utils/validation.hpp"

namespace sim {

EPSender::EPSender()
    : m_router(std::make_unique<RoutingModule>()) {}

bool EPSender::add_inlink(std::shared_ptr<ILink> link) {
    if (!is_valid_link(link)) {
        return false;
    }
    return m_router->add_inlink(link);
}

bool EPSender::add_outlink(std::shared_ptr<ILink> link) {
    if (!is_valid_link(link)) {
        return false;
    }
    m_router->add_outlink(link);
    return true;
}

bool EPSender::update_routing_table(Id dest_id, std::shared_ptr<ILink> link, size_t paths_count) {
    if (!is_valid_link(link)) {
        return false;
    }
    m_router->update_routing_table(dest_id, link, paths_count);
    return true;
}

std::shared_ptr<ILink> EPSender::next_inlink() {
    return m_router->next_inlink();
};

std::shared_ptr<ILink> EPSender::get_link_to_destination(Packet packet) const {
    return m_router->get_link_to_destination(packet);
};

DeviceType EPSender::get_type() const { return DeviceType::SENDER; }

void EPSender::enqueue_packet(Packet packet) {
    m_flow_buffer.push(packet);
    LOG_INFO(fmt::format("Packet {} arrived to sender", packet.to_string()));
}

Time EPSender::process() {
    std::shared_ptr<ILink> current_inlink = next_inlink();
    Time total_processing_time = 1;

    if (current_inlink == nullptr) {
        LOG_WARN("No available inlinks for device");
        return total_processing_time;
    }

    std::optional<Packet> opt_packet = current_inlink->get_packet();
    if (!opt_packet.has_value()) {
        LOG_WARN("No available packets from inlink for device");
        return total_processing_time;
    }

    Packet packet = opt_packet.value();
    if (packet.flow == nullptr) {
        LOG_ERROR("Packet flow does not exist");
        return total_processing_time;
    }

    LOG_INFO("Processing packet from link on sender. Packet: " +
             packet.to_string() + ". Sender id: " + std::to_string(get_id()) + ". Time: " + std::to_string(Scheduler::get_instance().get_current_time()));

    if (packet.dest_id == get_id()) {
        packet.flow->update(packet, get_type());
    } else {
        LOG_WARN(
            "Packet arrived to EPSender that is not its destination; use routing "
            "table to send it further");
        std::shared_ptr<ILink> next_link = get_link_to_destination(packet);

        if (next_link == nullptr) {
            LOG_WARN("No link corresponds to destination device");
            return total_processing_time;
        }
        next_link->schedule_arrival(packet);
    }
    // total_processing_time += processing_ack_time;

    return total_processing_time;
}

Time EPSender::send_data() {
    Time total_processing_time = 1;

    // TODO: wrap packet getting into some method (?)
    if (m_flow_buffer.empty()) {
        LOG_WARN("No packets to send");
        return total_processing_time;
    }
    Packet data_packet = m_flow_buffer.front();
    m_flow_buffer.pop();

    LOG_INFO("Taken new data packet on sender. Packet: " +
             data_packet.to_string() + ". Sender id: " + std::to_string(get_id()) + ". Time: " + std::to_string(Scheduler::get_instance().get_current_time()));

    auto next_link = get_link_to_destination(data_packet);
    if (next_link == nullptr) {
        LOG_WARN("Link to send data packet does not exist");
        return total_processing_time;
    }

    LOG_INFO("Sent new data packet from sender. Data packet: " +
             data_packet.to_string() + ". Sender id: " + std::to_string(get_id()) + ". Time: " + std::to_string(Scheduler::get_instance().get_current_time()));

    next_link->schedule_arrival(data_packet);
    // total_processing_time += sending_data_time;
    return total_processing_time;
}

Time EPSender::send_system_packet(Packet packet) {
    Time total_processing_time = 1;

    auto next_link = get_link_to_destination(packet);
    if (next_link == nullptr) {
        LOG_WARN("Link to send packet does not exist");
        return total_processing_time;
    }

    // TODO: add some sender ID for easier packet path tracing
    LOG_INFO("Sent new system packet from sender. Data packet: " +
        packet.to_string() + ". Sender id: " + std::to_string(get_id()) + ". Time: " + std::to_string(Scheduler::get_instance().get_current_time()));

    next_link->schedule_arrival(packet);
    // total_processing_time += sending_data_time;
    return total_processing_time;
}

std::set<std::shared_ptr<ILink>> EPSender::get_outlinks() {
    return m_router->get_outlinks();
}

Id EPSender::get_id() const { return m_router->get_id(); }

}  // namespace sim
