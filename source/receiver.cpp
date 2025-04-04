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

bool Receiver::add_inlink(std::shared_ptr<ILink> link) {
    if (link == nullptr) {
        spdlog::warn("Passed link is null");
        return false;
    }

    if (this != link->get_to().get()) {
        spdlog::warn("Link destination device is incorrect (expected current device)");
        return false;
    }
    m_router->add_inlink(link);
    return true;
}

bool Receiver::update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                                     std::shared_ptr<ILink> link) {
    if (link == nullptr) {
        spdlog::warn("Passed link is null");
        return false;
    }

    if (dest == nullptr) {
        spdlog::warn("Passed destination is null");
        return false;
    }

    if (this != link->get_from().get()) {
        spdlog::warn("Link source device is incorrect (expected current device)");
        return false;
    }
    m_router->update_routing_table(dest, link);
    return true;
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

std::uint32_t Receiver::process() {
    std::shared_ptr<ILink> current_inlink = m_router->next_inlink();
    std::uint32_t total_processing_time = 1;

    if (current_inlink == nullptr) {
        spdlog::warn("No available inlinks for device");
        return total_processing_time;
    }

    std::optional<Packet> opt_data_packet = current_inlink->get_packet();
    if (!opt_data_packet.has_value()) {
        spdlog::warn("No available packets from inlink for device");
        return total_processing_time;
    }

    Packet data_packet = opt_data_packet.value();
    if (data_packet.flow == nullptr) {
        // TODO: discuss do we need to discard packet in such scenario or process it
        spdlog::error("Packet flow does not exist");
        return total_processing_time;
    }

    std::shared_ptr<IReceiver> receiver = data_packet.flow->get_receiver();
    if (receiver.get() == this) {
        // TODO: think about processing time

    } else {
        spdlog::warn("Packet arrived to Receiver that is not its destination; using routing table to send it further");
        std::shared_ptr<ILink> next_link = get_link_to_destination(receiver); 

        if (next_link == nullptr) {
            spdlog::warn("No link corresponds to destination device");
            return total_processing_time;
        }
        next_link->schedule_arrival(data_packet);
    }
    // total_processing_time += processing_time;
    
    // TODO: move ack sending to function
    auto sender = data_packet.flow->get_sender();
    if (sender == nullptr) {
        spdlog::error("Flow sender does not exists");
        return total_processing_time;
    }

    Packet ack = {PacketType::ACK, 1, data_packet.flow};
    std::shared_ptr<ILink> link_to_src = m_router->get_link_to_destination(sender);
    if (link_to_src == nullptr) {
        spdlog::error("Link to send ack does not exist");
        return total_processing_time;
    }
    if(data_paket.get_destination().get() == this()) {
        // check that type of paket is DATA, replace data_paket to ask
     }
     // send paket using routing table
    // Not sure if we want to send ack before processing or after it
    link_to_src->schedule_arrival(ack);

    return total_processing_time;
}

}  // namespace sim
