#include "switch.hpp"

#include <spdlog/spdlog.h>

#include <memory>
#include <stdexcept>

#include "link.hpp"

#define LOG_WARN(msg) spdlog::warn("{} ({}:{})", msg, __FILE__, __LINE__)

namespace sim {

Switch::Switch() : m_router(std::make_unique<RoutingModule>()) {}

bool Switch::add_inlink(std::shared_ptr<ILink> link) {
    if (link == nullptr) {
        LOG_WARN("Add nullptr inlink to switch device");
        return false;
    }
    if (link->get_to().get() != this) {
        LOG_WARN("Inlink destination is not our device");
        return false;
    }
    return m_router->add_inlink(link);
}

bool Switch::update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                                  std::shared_ptr<ILink> link) {
    if (dest == nullptr) {
        LOG_WARN("Destination device does not exist");
        return false;
    }
    if (link == nullptr) {
        LOG_WARN("Link does not exist");
        return false;
    }
    if (link->get_from().get() != this) {
        LOG_WARN("Link source is not our device");
        return false;
    }
    return m_router->update_routing_table(dest, link);
}

std::vector<std::shared_ptr<IRoutingDevice>> Switch::get_neighbours() const {
    return m_router->get_neighbours();
}

std::shared_ptr<ILink> Switch::next_inlink() { return m_router->next_inlink(); }

std::shared_ptr<ILink> Switch::get_link_to_destination(
    std::shared_ptr<IRoutingDevice> dest) const {
    return m_router->get_link_to_destination(dest);
}

DeviceType Switch::get_type() const { return DeviceType::SWITCH; }

Time Switch::process() {
    std::shared_ptr<ILink> link = next_inlink();
    Time total_processing_time = 1;

    if (link == nullptr) {
        LOG_WARN("No next inlink");
        return total_processing_time;
    }

    std::optional<Packet> optional_packet = link->get_packet();
    if (!optional_packet.has_value()) {
        LOG_WARN("No packet in link");
        return total_processing_time;
    }
    Packet packet = optional_packet.value();
    if (packet.flow == nullptr) {
        LOG_WARN("No flow in packet");
        return total_processing_time;
    }
    std::shared_ptr<IReceiver> destination = packet.flow->get_receiver();

    std::shared_ptr<ILink> next_link = get_link_to_destination(destination);

    if (next_link == nullptr) {
        LOG_WARN("No link corresponds to destination device");
        return total_processing_time;
    }
    next_link->schedule_arrival(packet);
    return total_processing_time;
}

}  // namespace sim
