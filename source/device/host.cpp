#include "host.hpp"

#include <spdlog/fmt/fmt.h>

#include "logger/logger.hpp"
#include "utils/validation.hpp"

namespace sim {

Host::Host(Id a_id) : m_router(a_id) {}

bool Host::add_inlink(std::shared_ptr<ILink> link) {
    if (!is_valid_link(link)) {
        return false;
    }
    return m_router.add_inlink(link);
}

bool Host::add_outlink(std::shared_ptr<ILink> link) {
    if (!is_valid_link(link)) {
        return false;
    }
    m_router.add_outlink(link);
    return true;
}

bool Host::update_routing_table(Id dest_id, std::shared_ptr<ILink> link,
                                size_t paths_count) {
    if (!is_valid_link(link)) {
        return false;
    }
    m_router.update_routing_table(dest_id, link, paths_count);
    return true;
}

std::shared_ptr<ILink> Host::next_inlink() { return m_router.next_inlink(); };

std::shared_ptr<ILink> Host::get_link_to_destination(Packet packet) const {
    return m_router.get_link_to_destination(packet);
};

bool Host::notify_about_arrival(Time arrival_time) {
    return m_process_scheduler.notify_about_arriving(arrival_time,
                                                     weak_from_this());
};

DeviceType Host::get_type() const { return DeviceType::SENDER; }

void Host::enqueue_packet(Packet packet) {
    m_nic_buffer.push(packet);
    m_send_data_scheduler.notify_about_arriving(
        Scheduler::get_instance().get_current_time(), weak_from_this());
    LOG_INFO(fmt::format("Packet {} arrived to host", packet.to_string()));
}

Time Host::process() {
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

    // TODO: add some sender ID for easier packet path tracing
    LOG_INFO("Processing packet from link on host. Packet: " +
             packet.to_string());

    if (packet.dest_id == get_id()) {
        packet.flow->update(packet, get_type());
    } else {
        LOG_WARN(
            "Packet arrived to Host that is not its destination; use routing "
            "table to send it further");
        std::shared_ptr<ILink> next_link = get_link_to_destination(packet);

        if (next_link == nullptr) {
            LOG_WARN("No link corresponds to destination device");
            return total_processing_time;
        }
        next_link->schedule_arrival(packet);
    }

    Time current_time = Scheduler::get_instance().get_current_time();
    if (m_process_scheduler.notify_about_finish(current_time +
                                                total_processing_time)) {
        return 0;
    }

    return total_processing_time;
}

Time Host::send_packet() {
    Time total_processing_time = 1;

    if (m_nic_buffer.empty()) {
        LOG_WARN("No packets to send");
        return total_processing_time;
    }
    Packet data_packet = m_nic_buffer.front();
    m_nic_buffer.pop();

    LOG_INFO(fmt::format("Taken new data packet on host {}. Packet: {}",
                         get_id(), data_packet.to_string()));

    auto next_link = get_link_to_destination(data_packet);
    if (next_link == nullptr) {
        LOG_WARN("Link to send data packet does not exist");
        return total_processing_time;
    }

    LOG_INFO(fmt::format("Sent new packet from host. Packet: {}", get_id(),
                         data_packet.to_string()));

    next_link->schedule_arrival(data_packet);
    if (m_send_data_scheduler.notify_about_finish(
            Scheduler::get_instance().get_current_time() +
            total_processing_time)) {
        return 0;
    }

    return total_processing_time;
}

std::set<std::shared_ptr<ILink>> Host::get_outlinks() {
    return m_router.get_outlinks();
}

Id Host::get_id() const { return m_router.get_id(); }

}  // namespace sim
