#include "switch.hpp"

#include <memory>
#include <stdexcept>

#include "link.hpp"

namespace sim {

Switch::Switch() : m_router(std::make_unique<RoutingModule>()) {}

void Switch::add_inlink(std::shared_ptr<ILink> link) {
    m_router->add_inlink(link);
}

void Switch::update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                                  std::shared_ptr<ILink> link) {
    m_router->update_routing_table(dest, link);
}

std::shared_ptr<ILink> Switch::next_inlink() { return m_router->next_inlink(); }

std::shared_ptr<ILink> Switch::get_destination(
    std::shared_ptr<IRoutingDevice> dest) const {
    return m_router->get_destination(dest);
}

DeviceType Switch::get_type() const { return DeviceType::SWITCH; }

void Switch::process() {
    std::shared_ptr<ILink> link = m_router->next_inlink();

    // TODO: discuss this
    if (link == nullptr) {
        throw std::runtime_error("No next inlink");
    }

    Packet packet = link->get_packet();
    IReceiver* destination = packet.flow->get_destination();
    // TODO:: remove creating smart pointer from here
    std::shared_ptr<ILink> next_link =
        get_destination(std::shared_ptr<IRoutingDevice>(destination));

    // TODO: discuss this
    if (next_link == nullptr) {
        // TODO: write warning to log
        return;
    }
    next_link->schedule_arrival(packet);
}

}  // namespace sim
