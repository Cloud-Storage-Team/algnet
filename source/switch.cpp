#include "switch.hpp"

#include <memory>
#include <stdexcept>

#include "link.hpp"

namespace sim {

Switch::Switch() : m_router(std::make_unique<RoutingModule>()) {}

void Switch::add_inlink(std::shared_ptr<ILink> link) {
    if (link == nullptr) {
        // TODO: log warning
        throw std::runtime_error("Link does not exists");
    }
    if (link->get_to().get() != this) {
        // TODO: log warning
        throw std::runtime_error("Link does not exists");
    }
    m_router->add_inlink(link);
}

void Switch::update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                                  std::shared_ptr<ILink> link) {
    if (dest == nullptr || link == nullptr) {
        // TODO: log warning
        throw std::runtime_error("Link or device does not exists");
    }
    if (link->get_from().get() != this) {
        // TODO: log warning
        throw std::runtime_error("Link source is not our device");
    }
    m_router->update_routing_table(dest, link);
}

std::shared_ptr<ILink> Switch::next_inlink() { return m_router->next_inlink(); }

std::shared_ptr<ILink> Switch::get_link_to_destination(
    std::shared_ptr<IRoutingDevice> dest) const {
    return m_router->get_link_to_destination(dest);
}

DeviceType Switch::get_type() const { return DeviceType::SWITCH; }

void Switch::process() {
    std::shared_ptr<ILink> link = next_inlink();

    // TODO: discuss this
    if (link == nullptr) {
        throw std::runtime_error("No next inlink");
    }

    Packet packet = link->get_packet();
    if (packet.flow == nullptr) {
        // TODO: change runtime_error to log warning
        throw std::runtime_error("No flow in packet");
    }
    std::shared_ptr<IReceiver> destination =
        std::shared_ptr<IReceiver>(packet.flow->get_destination());
    // TODO:: remove creating smart pointer from here
    std::shared_ptr<ILink> next_link = get_link_to_destination(destination);

    // TODO: discuss this
    if (next_link == nullptr) {
        // TODO: replace error thow to warning logging
        throw std::runtime_error("No link corresponds to destination device");
    }
    next_link->schedule_arrival(packet);
}

}  // namespace sim
