#include "receiver.hpp"

#include <iostream>

#include "event.hpp"
#include "flow.hpp"
#include "link.hpp"
#include "packet.hpp"
#include "scheduler.hpp"

namespace sim {

void Receiver::add_inlink(std::shared_ptr<ILink> link) {
    if (this != link->get_to().get()) {
        // Add log
        return;
    }
    m_router->add_inlink(link);
}

void Receiver::update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                                     std::shared_ptr<ILink> link) {
    if (this != link->get_from().get()) {
        // Add log
        return;
    }
    m_router->update_routing_table(dest, link);
}

DeviceType Receiver::get_type() const {
    return DeviceType::RECEIVER;
}

void Receiver::process() {
    std::shared_ptr<ILink> current_inlink = m_router->next_inlink();
    std::uint32_t total_processing_time = 0;

    // TODO: process current_inlink = nullptr

    std::optional<Packet> opt_data_packet = current_inlink->get_packet();

    // TODO: process no packet

    Packet data_packet = opt_data_packet.value();

    // processing...
    // total_processing_time += processing_time;


    // TODO: send ack
    // Packet ack = {PacketType::ACK, 1, data_packet.flow};
    // std::shared_ptr<ILink> link_to_src = m_router->get  m_routing_table[data_packet.flow->get_source()];
    // // Not sure if we want to send ack before processing or after it
    // link_to_src->schedule_arrival(ack);

    // return total_processing_time;
}

}  // namespace sim
