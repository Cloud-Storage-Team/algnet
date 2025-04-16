#include "receiver_mock.hpp"

#include <memory>

#include "link.hpp"

namespace test {

bool ReceiverMock::add_inlink(std::shared_ptr<sim::ILink> link) {
    return false;
}

bool ReceiverMock::add_outlink(std::shared_ptr<sim::ILink> link) {
    return false;
}

bool ReceiverMock::update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                                        std::shared_ptr<sim::ILink> link) {
    return false;
}

std::weak_ptr<sim::ILink> ReceiverMock::next_inlink() { return {}; }

std::weak_ptr<sim::ILink> ReceiverMock::get_link_to_destination(
    std::weak_ptr<IRoutingDevice> device) const {
    return {};
}

std::uint32_t ReceiverMock::process() { return 1; }
sim::DeviceType ReceiverMock::get_type() const {
    return sim::DeviceType::RECEIVER;
}

std::set<std::weak_ptr<sim::ILink>, std::owner_less<std::weak_ptr<sim::ILink>>>
ReceiverMock::get_outlinks() const {
    return {};
}

}  // namespace test
