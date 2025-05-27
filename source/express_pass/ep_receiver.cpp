#include "ep_receiver.hpp"

#include <memory>
#include <iostream>

#include "scheduler.hpp"
#include "utils/validation.hpp"

namespace sim {

EPReceiver::EPReceiver()
    : m_router(std::make_unique<RoutingModule>()) {}

bool EPReceiver::add_inlink(std::shared_ptr<ILink> link) {
    if (!is_valid_link(link)) {
        return false;
    }
    return m_router->add_inlink(link);
}

bool EPReceiver::add_outlink(std::shared_ptr<ILink> link) {
    if (!is_valid_link(link)) {
        return false;
    }
    return m_router->add_outlink(link);
}

bool EPReceiver::update_routing_table(Id dest_id, std::shared_ptr<ILink> link, size_t paths_count) {
    if (!is_valid_link(link)) {
        return false;
    }
    return m_router->update_routing_table(dest_id, link, paths_count);
}

std::shared_ptr<ILink> EPReceiver::next_inlink() {
    return m_router->next_inlink();
};

std::shared_ptr<ILink> EPReceiver::get_link_to_destination(Packet packet) const {
    return m_router->get_link_to_destination(packet);
};

DeviceType EPReceiver::get_type() const { return DeviceType::RECEIVER; }

Time EPReceiver::process() {
    std::shared_ptr<ILink> current_inlink = next_inlink();
    Time total_processing_time = 10;

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

    // TODO: add some receiver ID for easier packet path tracing
    LOG_INFO("Processing packet from link on receiver. Packet: " +
        packet.to_string() + ". Receiver id: " + std::to_string(get_id()) + ". Time: " + std::to_string(Scheduler::get_instance().get_current_time()));

    if (packet.dest_id == get_id()) {
        // TODO: think about processing time   
        packet.flow->update(packet, get_type());
        m_cnt++;
        // std::cout << "GOT: " << ++m_cnt <<  " TIME: " << Scheduler::get_instance().get_current_time() << std::endl;
    } else {
        LOG_WARN(
            "Packet arrived to EPReceiver that is not its destination; using "
            "routing table to send it further");
        std::shared_ptr<ILink> next_link = get_link_to_destination(packet);

        if (next_link == nullptr) {
            LOG_WARN("No link corresponds to destination device");
            return total_processing_time;
        }
        next_link->schedule_arrival(packet);
        // TODO: think about redirecting time
    }

    return total_processing_time;
}

Time EPReceiver::send_system_packet(Packet packet) {
    Time total_processing_time = 1;

    auto next_link = get_link_to_destination(packet);
    if (next_link == nullptr) {
        LOG_WARN("Link to send packet does not exist. Packet: " + packet.to_string());
        return total_processing_time;
    }

    // TODO: add some sender ID for easier packet path tracing
    LOG_INFO("Sent new system packet from receiver. Data packet: " +
        packet.to_string() + ". Receiver id: " + std::to_string(get_id()) + ". Time: " + std::to_string(Scheduler::get_instance().get_current_time()));

    next_link->schedule_arrival(packet);
    // total_processing_time += sending_data_time;
    return total_processing_time;
}

std::set<std::shared_ptr<ILink>> EPReceiver::get_outlinks() {
    return m_router->get_outlinks();
}

Id EPReceiver::get_id() const { return m_router->get_id(); }

}  // namespace sim
