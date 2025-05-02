#include "express_pass_receiver.hpp"

#include "event.hpp"
#include "link.hpp"
#include "logger/logger.hpp"
#include "utils/identifier_factory.hpp"
#include "utils/validation.hpp"

namespace sim {

ExpressPassReceiver::ExpressPassReceiver()
    : m_router(std::make_unique<RoutingModule>()),
      m_id(IdentifierFactory::get_instance().generate_id()) {}

bool ExpressPassReceiver::add_inlink(std::shared_ptr<ILink> link) {
    if (!is_valid_link(link)) {
        return false;
    }

    if (this != link->get_to().get()) {
        LOG_WARN(
            "Link destination device is incorrect (expected current device)");
        return false;
    }
    return m_router->add_inlink(link);
}

bool ExpressPassReceiver::add_outlink(std::shared_ptr<ILink> link) {
    if (!is_valid_link(link)) {
        return false;
    }

    if (this != link->get_from().get()) {
        LOG_WARN("Outlink source is not our device");
        return false;
    }
    return m_router->add_outlink(link);
}

bool ExpressPassReceiver::update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                                    std::shared_ptr<ILink> link, int paths) {
    if (dest == nullptr) {
        LOG_WARN("Passed destination is null");
        return false;
    }

    if (!is_valid_link(link)) {
        return false;
    }

    if (this != link->get_from().get()) {
        LOG_WARN("Link source device is incorrect (expected current device)");
        return false;
    }
    return m_router->update_routing_table(dest, link, paths);
}

std::shared_ptr<ILink> ExpressPassReceiver::next_inlink() {
    return m_router->next_inlink();
};

std::shared_ptr<ILink> ExpressPassReceiver::get_link_to_destination(Packet packet) const {
    return m_router->get_link_to_destination(packet);
};

DeviceType ExpressPassReceiver::get_type() const { return DeviceType::RECEIVER; }

Time ExpressPassReceiver::process(Time current_time) {
    (void) current_time;
    std::shared_ptr<ILink> current_inlink = m_router->next_inlink();
    Time total_processing_time = 1;

    if (current_inlink == nullptr) {
        LOG_WARN("No available inlinks for device");
        return total_processing_time;
    }

    std::optional<Packet> opt_packet = current_inlink->get_packet();
    if (!opt_packet.has_value()) {
        LOG_TRACE("No available packets from inlink for device");
        return total_processing_time;
    }

    Packet packet = opt_packet.value();
    if (packet.flow == nullptr) {
        LOG_ERROR("Packet flow does not exist");
        return total_processing_time;
    }

    // TODO: add some receiver ID for easier packet path tracing
    LOG_INFO("Processing packet from link on receiver. Packet: " +
        packet.to_string() + ". Receiver id: " + std::to_string(get_id()) + ". Time: " + std::to_string(current_time));

    std::shared_ptr<IRoutingDevice> destination = packet.get_destination();
    if (destination == nullptr) {
        LOG_WARN("Destination device pointer is expired");
        return total_processing_time;
    }
    if (destination.get() == this) {
        // TODO: think about processing time
        packet.flow->update(current_time, packet, get_type());
    } else {
        LOG_WARN(
            "Packet arrived to ExpressPassReceiver that is not its destination; using "
            "routing table to send it further");
        std::shared_ptr<ILink> next_link = get_link_to_destination(packet);

        if (next_link == nullptr) {
            LOG_WARN("No link corresponds to destination device");
            return total_processing_time;
        }
        next_link->schedule_arrival(current_time, packet);
        // TODO: think about redirecting time
    }

    return total_processing_time;
}

Time ExpressPassReceiver::send_system_packet(Time current_time, Packet packet) {
    Time total_processing_time = 1;

    auto next_link = get_link_to_destination(packet);
    if (next_link == nullptr) {
        LOG_WARN("Link to send packet does not exist. Packet: " + packet.to_string());
        return total_processing_time;
    }

    // TODO: add some sender ID for easier packet path tracing
    LOG_INFO("Sent new system packet from receiver. Data packet: " +
        packet.to_string() + ". Receiver id: " + std::to_string(get_id()) + ". Time: " + std::to_string(current_time));

    next_link->schedule_arrival(current_time, packet);
    // total_processing_time += sending_data_time;
    return total_processing_time;
}

std::set<std::shared_ptr<ILink>> ExpressPassReceiver::get_outlinks() {
    return m_router->get_outlinks();
}

Id ExpressPassReceiver::get_id() const { return m_id; }

}  // namespace sim
