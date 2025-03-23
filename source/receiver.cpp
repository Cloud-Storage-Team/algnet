#include "receiver.hpp"

#include <spdlog/spdlog.h>

#include "link.hpp"

namespace sim {

void Receiver::add_inlink(std::shared_ptr<ILink> link) {}

void Receiver::update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                                    std::shared_ptr<ILink> link) {}

std::vector<std::shared_ptr<IRoutingDevice>> Receiver::get_neighbours() const {
    return {};
};

std::shared_ptr<ILink> Receiver::next_inlink() { return nullptr; };

std::shared_ptr<ILink> Receiver::get_link_to_destination(
    std::shared_ptr<IRoutingDevice> dest) const {
    return nullptr;
};

DeviceType Receiver::get_type() const { return DeviceType::RECEIVER; }

std::vector<std::shared_ptr<ILink>> Receiver::get_outlinks() const {
    return {};
}

void Receiver::process() {}

}  // namespace sim
