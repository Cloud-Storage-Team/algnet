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

void Sender::process() {
    // std::uint32_t current_time = start_time;

    // TODO: Handle empty m_inlinks
    Link* current_inlink = (*m_next_inlink);
    if (++m_next_inlink == m_inlinks.end()) {
        m_next_inlink = m_inlinks.begin();
    }

    // TODO: Handle absence of packet (return optional)
    Packet packet = current_inlink->get_packet();
    if (packet.type == PacketType::ACK) {
        packet.flow->update();
    }
    // current_time += processing_ack_time;

    if (m_flow_buffer.empty()) {
        return;
    }

    // TODO: wrap into some method
    Packet data_packet = m_flow_buffer.front();
    m_flow_buffer.pop();

    // TODO: think about spliting logic of handling acks and sending data
    Device* destination = packet.flow->get_destination();
    Link* next_link = m_routing_table[destination];
    next_link->schedule_arrival(data_packet);
    // current_time += sending_data_time;


    Process new_process_event = Process(this);
    // new_process_event.time = current_time;
    Scheduler::get_instance().add(new_process_event);

}

}  // namespace sim
