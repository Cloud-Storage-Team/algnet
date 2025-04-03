#include "sender.hpp"

#include <spdlog/spdlog.h>

#include "link.hpp"
#include "event.hpp"
#include "scheduler.hpp"

namespace sim {

Sender::Sender() : m_router(std::make_unique<RoutingModule>()) {}

void Sender::add_inlink(std::shared_ptr<ILink> link) {
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

void Sender::update_routing_table(std::shared_ptr<IRoutingDevice> dest,
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

std::vector<std::shared_ptr<IRoutingDevice>> Sender::get_neighbours() const {
    return m_router->get_neighbours();
};

std::shared_ptr<ILink> Sender::next_inlink() {
    return m_router->next_inlink();
};

std::shared_ptr<ILink> Sender::get_link_to_destination(
    std::shared_ptr<IRoutingDevice> dest) const {
        return m_router->get_link_to_destination(dest);
};

DeviceType Sender::get_type() const {
    return DeviceType::SENDER;
}

void Sender::enqueue_packet(Packet packet) {
    m_flow_buffer.push(packet);
}

std::uint32_t Sender::process() {
    std::shared_ptr<ILink> current_inlink = m_router->next_inlink();
    std::uint32_t total_processing_time = 1;

    if (current_inlink == nullptr) {
        spdlog::warn("No available inlinks for device");
        return total_processing_time;
    }

    std::optional<Packet> opt_packet = current_inlink->get_packet();
    if (!opt_packet.has_value()) {
        spdlog::warn("No available packets from inlink for device");
        return total_processing_time;
    }

    Packet packet = opt_packet.value();
    if (packet.flow == nullptr) {
        spdlog::error("Packet flow does not exist");
        return total_processing_time;
    }

    if (packet.type == PacketType::ACK) {
         std::shared_ptr<IReceiver> destination = packet.flow->get_destination();
         if (destination.get() == this) {
                 packet.flow->update();
         } else {
             spwlog::warn("Packet arrived to Sender that is not its destination; use routing table to send it further");
             std::shared_ptr<ILink> next_link = get_link_to_destination(destination); 

             if (next_link == nullptr) {
                 spdlog::warn("No link corresponds to destination device");
                 return;
             }
             next_link->schedule_arrival(packet);
         }
        // total_processing_time += processing_ack_time;
    }

    return total_processing_time;
}

std::uint32_t Sender::send_data() {
    std::uint32_t total_processing_time = 1;
    
    // TODO: wrap into some method (?)
    if (m_flow_buffer.empty()) {
        spdlog::warn("No packets to send");
        return total_processing_time;
    }
    Packet data_packet = m_flow_buffer.front();
    m_flow_buffer.pop();

    auto destination = data_packet.flow->get_destination();
    auto next_link = m_router->get_link_to_destination(destination);
    if (next_link == nullptr) {
        spdlog::warn("Link to send data packet does not exist");
        return total_processing_time;
    }
    next_link->schedule_arrival(data_packet);
    // total_processing_time += sending_data_time;
    return total_processing_time;
}

}  // namespace sim
