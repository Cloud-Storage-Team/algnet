#include "switch.hpp"

#include <memory>

#include "link.hpp"
#include "logger/logger.hpp"
#include "utils/validation.hpp"

namespace sim {

Switch::Switch()
    : m_router(std::make_unique<RoutingModule>()),
      m_id(IdentifierFactory::get_instance().generate_id()) {}

bool Switch::add_inlink(std::shared_ptr<ILink> link) {
    if (!is_valid_link(link)) {
        return false;
    }
    if (link->get_to().get() != this) {
        LOG_WARN("Inlink destination is not our device");
        return false;
    }
    return m_router->add_inlink(link);
}

bool Switch::add_outlink(std::shared_ptr<ILink> link) {
    if (!is_valid_link(link)) {
        return false;
    }
    if (link->get_from().get() != this) {
        LOG_WARN("Outlink source is not our device");
        return false;
    }
    return m_router->add_outlink(link);
}

bool Switch::update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                                  std::shared_ptr<ILink> link) {
    if (dest == nullptr) {
        LOG_WARN("Destination device does not exist");
        return false;
    }
    if (!is_valid_link(link)) {
        return false;
    }
    if (link->get_from().get() != this) {
        LOG_WARN("Link source is not our device");
        return false;
    }
    return m_router->update_routing_table(dest, link);
}

std::weak_ptr<ILink> Switch::next_inlink() { return m_router->next_inlink(); }

std::shared_ptr<ILink> Switch::get_link_to_destination(
    std::shared_ptr<IRoutingDevice> dest) const {
    return m_router->get_link_to_destination(dest);
}

DeviceType Switch::get_type() const { return DeviceType::SWITCH; }

Time Switch::process() {
    Time total_processing_time = 1;
    std::weak_ptr<ILink> link = next_inlink();

    if (link.expired()) {
        LOG_WARN("No next inlink");
        return total_processing_time;
    }

    std::optional<Packet> optional_packet = link.lock()->get_packet();
    if (!optional_packet.has_value()) {
        LOG_WARN("No packet in link");
        return total_processing_time;
    }
    Packet packet = optional_packet.value();
    if (packet.flow == nullptr) {
        LOG_WARN("No flow in packet");
        return total_processing_time;
    }
    std::shared_ptr<IRoutingDevice> destination = packet.get_destination();
    if (destination == nullptr) {
        LOG_WARN("Destination device pointer is expired");
        return total_processing_time;
    }

    std::shared_ptr<ILink> next_link = get_link_to_destination(destination);

    if (next_link == nullptr) {
        LOG_WARN("No link corresponds to destination device");
        return total_processing_time;
    }

    // TODO: add some switch ID for easier packet path tracing
    LOG_INFO("Processing packet from link on switch. Packet: " +
             packet.to_string());

    // TODO: increase total_processing_time correctly
    next_link->schedule_arrival(packet);
    return total_processing_time;
}

std::set<std::weak_ptr<ILink>, std::owner_less<std::weak_ptr<ILink>>>
Switch::get_outlinks() const {
    return m_router->get_outlinks();
}

Id Switch::get_id() const { return m_id; }

}  // namespace sim
