#include "receiver.hpp"

#include <spdlog/spdlog.h>
#include <iostream>

#include "event.hpp"
#include "flow.hpp"
#include "link.hpp"
#include "packet.hpp"
#include "scheduler.hpp"

namespace sim {

Receiver::Receiver() : m_router(std::make_unique<RoutingModule>()) {}

void Receiver::add_inlink(std::shared_ptr<ILink> link) {
    if (link == nullptr) {
        spdlog::warn("Passed link is null");
        return;
    }

    if (this != link->get_to().get()) {
        spdlog::warn("Link destination device is incorrect (expected current device)");
        return;
    }
    m_router->add_inlink(link);
}

void Receiver::update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                                     std::shared_ptr<ILink> link) {
    if (link == nullptr) {
        spdlog::warn("Passed link is null");
        return;
    }

    if (dest == nullptr) {
        spdlog::warn("Passed destination is null");
        return;
    }

    if (this != link->get_from().get()) {
        spdlog::warn("Link source device is incorrect (expected current device)");
        return;
    }
    m_router->update_routing_table(dest, link);
}

std::vector<std::shared_ptr<IRoutingDevice>> Receiver::get_neighbours() const {
    return m_router->get_neighbours();
};

std::shared_ptr<ILink> Receiver::next_inlink() {
    return m_router->next_inlink();
};

std::shared_ptr<ILink> Receiver::get_link_to_destination(
    std::shared_ptr<IRoutingDevice> dest) const {
        return m_router->get_link_to_destination(dest);
};

DeviceType Receiver::get_type() const {
    return DeviceType::RECEIVER;
}

void Receiver::process() {
    std::shared_ptr<ILink> current_inlink = m_router->next_inlink();
    std::uint32_t total_processing_time = 0;

    if (current_inlink == nullptr) {
        spdlog::warn("No available inlinks for device");
        return;
    }

    std::optional<Packet> opt_data_packet = current_inlink->get_packet();
    if (!opt_data_packet.has_value()) {
        spdlog::warn("No available packets from inlink for device");
        return;
    }

    Packet data_packet = opt_data_packet.value();
    if (data_packet.flow == nullptr) {
        spdlog::warn("Packet flow does not exist");
        return;
    }
auto source = data_packet.flow->get_source();
if (source == nullptr) {
    spdlog::error("Flow destination does not exists")
    return;
}
    // processing...
    // total_processing_time += processing_time;

    Packet ack = {PacketType::ACK, 1, data_packet.flow};
    std::shared_ptr<ILink> link_to_src = m_router->get_link_to_destination(data_packet.flow->get_source());
    if (link_to_src == nullptr) {
        spdlog::warn("Link to send ack does not exist");
        return;
    }

    // Not sure if we want to send ack before processing or after it
    link_to_src->schedule_arrival(ack);

    // return total_processing_time;
}

}  // namespace sim
