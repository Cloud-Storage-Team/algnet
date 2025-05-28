#include "sender.hpp"

#include <spdlog/fmt/fmt.h>

#include <memory>

#include "event.hpp"
#include "link.hpp"
#include "logger/logger.hpp"
#include "utils/validation.hpp"

namespace sim {

Sender::Sender(Id a_id) : m_router(std::make_unique<RoutingModule>(a_id)) {}

bool Sender::add_inlink(std::shared_ptr<ILink> link) {
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

bool Sender::add_outlink(std::shared_ptr<ILink> link) {
    if (!is_valid_link(link)) {
        return false;
    }
    if (this != link->get_from().get()) {
        LOG_WARN("Outlink source is not our device");
        return false;
    }
    m_router->add_outlink(link);
    return true;
}

bool Sender::update_routing_table(std::shared_ptr<IRoutingDevice> dest,
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
    m_router->update_routing_table(dest, link, paths_count);
    return true;
}

std::shared_ptr<ILink> Sender::next_inlink() {
    return m_router->next_inlink();
};

std::shared_ptr<ILink> Sender::get_link_to_destination(
    std::shared_ptr<IRoutingDevice> dest) const {
    return m_router->get_link_to_destination(dest);
};

DeviceType Sender::get_type() const { return DeviceType::SENDER; }

void Sender::enqueue_packet(Packet packet) {
    m_flow_buffer.push(packet);
    LOG_INFO(fmt::format("Packet {} arrived to sender", packet.to_string()));
}

Time Sender::process() {
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
    LOG_INFO("Processing packet from link on sender. Packet: " +
             packet.to_string());

    auto destination = packet.get_destination();
    if (destination == nullptr) {
        LOG_WARN("Destination device pointer is expired");
        return total_processing_time;
    }
    if (packet.type == PacketType::ACK && destination.get() == this) {
        packet.flow->update(packet, get_type());
    } else {
        LOG_WARN(
            "Packet arrived to Sender that is not its destination; use routing "
            "table to send it further");
        std::shared_ptr<ILink> next_link = get_link_to_destination(destination);

        if (next_link == nullptr) {
            LOG_WARN("No link corresponds to destination device");
            return total_processing_time;
        }
        next_link->schedule_arrival(packet);
    }
    // total_processing_time += processing_ack_time;

    return total_processing_time;
}

Time Sender::send_data() {
    Time total_processing_time = 1;

    // TODO: wrap packet getting into some method (?)
    if (m_flow_buffer.empty()) {
        LOG_WARN("No packets to send");
        return total_processing_time;
    }
    Packet data_packet = m_flow_buffer.front();
    m_flow_buffer.pop();

    // TODO: add some sender ID for easier packet path tracing
    LOG_INFO("Taken new data packet on sender. Packet: " +
             data_packet.to_string());

    auto next_link = get_link_to_destination(data_packet.get_destination());
    if (next_link == nullptr) {
        LOG_WARN("Link to send data packet does not exist");
        return total_processing_time;
    }

    // TODO: add some sender ID for easier packet path tracing
    LOG_INFO("Sent new data packet from sender. Data packet: " +
             data_packet.to_string());

    next_link->schedule_arrival(data_packet);
    // total_processing_time += sending_data_time;
    return total_processing_time;
}

std::set<std::shared_ptr<ILink>> Sender::get_outlinks() {
    return m_router->get_outlinks();
}

Id Sender::get_id() const { return m_router->get_id(); }

}  // namespace sim
