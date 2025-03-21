#include "receiver_mock.hpp"

namespace test {

void ReceiverMock::add_inlink(std::shared_ptr<sim::ILink> link) {}

void ReceiverMock::update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                                        std::shared_ptr<sim::ILink> link) {}

std::shared_ptr<sim::ILink> ReceiverMock::next_inlink() { return nullptr; }

std::shared_ptr<sim::ILink> ReceiverMock::get_link_to_destination(
    std::shared_ptr<IRoutingDevice> dest) const {
    return nullptr;
}

void ReceiverMock::process() {}
sim::DeviceType ReceiverMock::get_type() const {
    return sim::DeviceType::RECEIVER;
}

std::vector<std::shared_ptr<sim::ILink>> ReceiverMock::get_outlinks() const {
    return {};
}

}  // namespace test
