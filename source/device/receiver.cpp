#include "receiver.hpp"

#include <memory>

#include "event.hpp"
#include "link.hpp"
#include "logger/logger.hpp"
#include "utils/identifier_factory.hpp"
#include "utils/validation.hpp"

namespace sim {

Receiver::Receiver(Id a_id) : m_router(std::make_unique<RoutingModule>(a_id)) {}

bool Receiver::add_inlink(std::shared_ptr<ILink> link) {
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

bool Receiver::add_outlink(std::shared_ptr<ILink> link) {
    if (!is_valid_link(link)) {
        return false;
    }
    if (this != link->get_from().get()) {
        LOG_WARN("Outlink source is not our device");
        return false;
    }
    return m_router->add_outlink(link);
}

bool Receiver::update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                                    std::shared_ptr<ILink> link,
                                    size_t paths_count) {
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
    return m_router->update_routing_table(dest, link, paths_count);
}

std::shared_ptr<ILink> Receiver::next_inlink() {
    return m_router->next_inlink();
};

std::shared_ptr<ILink> Receiver::get_link_to_destination(
    std::shared_ptr<IRoutingDevice> dest) const {
    return m_router->get_link_to_destination(dest);
};

DeviceType Receiver::get_type() const { return DeviceType::RECEIVER; }

Time Receiver::process() {
    std::shared_ptr<ILink> current_inlink = next_inlink();
    Time total_processing_time = 1;

    if (current_inlink == nullptr) {
        LOG_WARN("No available inlinks for device");
        return total_processing_time;
    }

    std::optional<Packet> opt_data_packet = current_inlink->get_packet();
    if (!opt_data_packet.has_value()) {
        LOG_WARN("No available packets from inlink for device");
        return total_processing_time;
    }

    Packet data_packet = opt_data_packet.value();
    if (data_packet.flow == nullptr) {
        LOG_ERROR("Packet flow does not exist");
        return total_processing_time;
    }

    // TODO: add some receiver ID for easier packet path tracing
    LOG_INFO("Processing packet from link on receiver. Packet: " +
             data_packet.to_string());

    std::shared_ptr<IRoutingDevice> destination = data_packet.get_destination();
    if (destination == nullptr) {
        LOG_WARN("Destination device pointer is expired");
        return total_processing_time;
    }
    if (data_packet.type == DATA && destination.get() == this) {
        // TODO: think about processing time
        // Not sure if we want to send ack before processing or after it
        total_processing_time += send_ack(data_packet);
    } else {
        LOG_WARN(
            "Packet arrived to Receiver that is not its destination; using "
            "routing table to send it further");
        std::shared_ptr<ILink> next_link = get_link_to_destination(destination);

        if (next_link == nullptr) {
            LOG_WARN("No link corresponds to destination device");
            return total_processing_time;
        }
        next_link->schedule_arrival(data_packet);
        // TODO: think about redirecting time
    }

    return total_processing_time;
}

Time Receiver::send_ack(Packet data_packet) {
    Time processing_time = 1;
    Packet ack(PacketType::ACK, 1, data_packet.flow, data_packet.send_time);

    auto destination = ack.get_destination();
    if (destination == nullptr) {
        LOG_ERROR("Ack destination does not exists");
        return processing_time;
    }

    std::shared_ptr<ILink> link_to_dest = get_link_to_destination(destination);
    if (link_to_dest == nullptr) {
        LOG_ERROR("Link to send ack does not exist");
        return processing_time;
    }

    // TODO: add some receiver ID for easier packet path tracing
    LOG_INFO("Sent ack after processing packet on receiver. Data packet: " +
             data_packet.to_string() + ". Ack packet: " + ack.to_string());

    link_to_dest->schedule_arrival(ack);
    return processing_time;
}

std::set<std::shared_ptr<ILink>> Receiver::get_outlinks() {
    return m_router->get_outlinks();
}

Id Receiver::get_id() const { return m_router->get_id(); }

}  // namespace sim
