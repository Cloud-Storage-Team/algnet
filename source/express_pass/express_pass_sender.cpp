#include "express_pass_sender.hpp"

#include <spdlog/fmt/fmt.h>

#include <memory>

#include "event.hpp"
#include "link.hpp"
#include "logger/logger.hpp"
#include "utils/validation.hpp"

namespace sim {

ExpressPassSender::ExpressPassSender()
    : m_router(std::make_unique<RoutingModule>()),
      m_id(IdentifierFactory::get_instance().generate_id()) {}

bool ExpressPassSender::add_inlink(std::shared_ptr<ILink> link) {
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

bool ExpressPassSender::add_outlink(std::shared_ptr<ILink> link) {
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

bool ExpressPassSender::update_routing_table(std::shared_ptr<IRoutingDevice> dest,
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
    m_router->update_routing_table(dest, link, paths);
    return true;
}

std::shared_ptr<ILink> ExpressPassSender::next_inlink() {
    return m_router->next_inlink();
};

std::shared_ptr<ILink> ExpressPassSender::get_link_to_destination(Packet packet) const {
    return m_router->get_link_to_destination(packet);
};

DeviceType ExpressPassSender::get_type() const { return DeviceType::SENDER; }

void ExpressPassSender::enqueue_packet(Packet packet) {
    m_flow_buffer.push(packet);
    LOG_INFO(fmt::format("Packet {} arrived to sender", packet.to_string()) + ". Sender id: " + std::to_string(get_id()));
}

Time ExpressPassSender::process(Time current_time) {
    (void)current_time;
    std::shared_ptr<ILink> current_inlink = m_router->next_inlink();
    Time total_processing_time = 1000;

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

    LOG_INFO("Processing packet from link on sender. Packet: " +
             packet.to_string() + ". Sender id: " + std::to_string(get_id()) + ". Time: " + std::to_string(current_time));

    auto destination = packet.get_destination();
    if (destination == nullptr) {
        LOG_WARN("Destination device pointer is expired");
        return total_processing_time;
    }
    if (destination.get() == this) {
        packet.flow->update(current_time, packet, get_type());
    } else {
        LOG_WARN(
            "Packet arrived to Sender that is not its destination; use routing "
            "table to send it further");
        std::shared_ptr<ILink> next_link = get_link_to_destination(packet);

        if (next_link == nullptr) {
            LOG_WARN("No link corresponds to destination device");
            return total_processing_time;
        }
        next_link->schedule_arrival(current_time, packet);
    }
    // total_processing_time += processing_ack_time;

    return total_processing_time;
}

Time ExpressPassSender::send_data(Time current_time) {
    (void)current_time;
    Time total_processing_time = 1000;

    // TODO: wrap packet getting into some method (?)
    if (m_flow_buffer.empty()) {
        LOG_TRACE("No packets to send");
        return total_processing_time;
    }
    Packet data_packet = m_flow_buffer.front();
    m_flow_buffer.pop();

    // TODO: add some sender ID for easier packet path tracing
    LOG_INFO("Taken new data packet on sender. Packet: " +
             data_packet.to_string() + ". Sender id: " + std::to_string(get_id()) + ". Time: " + std::to_string(current_time));

    auto next_link = get_link_to_destination(data_packet);
    if (next_link == nullptr) {
        LOG_WARN("Link to send data packet does not exist");
        return total_processing_time;
    }

    // TODO: add some sender ID for easier packet path tracing
    LOG_INFO("Sent new data packet from sender. Data packet: " +
             data_packet.to_string() + ". Sender id: " + std::to_string(get_id()) + ". Time: " + std::to_string(current_time));

    next_link->schedule_arrival(current_time, data_packet);
    // total_processing_time += sending_data_time;
    return total_processing_time;
}

Time ExpressPassSender::send_system_packet(Time current_time, Packet packet) {
    (void)current_time;
    Time total_processing_time = 1;

    auto next_link = get_link_to_destination(packet);
    if (next_link == nullptr) {
        LOG_WARN("Link to send packet does not exist");
        return total_processing_time;
    }

    // TODO: add some sender ID for easier packet path tracing
    LOG_INFO("Sent new system packet from sender. Data packet: " +
        packet.to_string() + ". Sender id: " + std::to_string(get_id()) + ". Time: " + std::to_string(current_time));

    next_link->schedule_arrival(current_time, packet);
    // total_processing_time += sending_data_time;
    return total_processing_time;
}

std::set<std::shared_ptr<ILink>> ExpressPassSender::get_outlinks() {
    return m_router->get_outlinks();
}

Id ExpressPassSender::get_id() const { return m_id; }

}  // namespace sim
