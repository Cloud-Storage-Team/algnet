#include "receiver_mock.hpp"

namespace test {

bool ReceiverMock::add_inlink(std::shared_ptr<sim::ILink> link) {
    return false;
}

bool ReceiverMock::update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                                        std::shared_ptr<sim::ILink> link) {
    return false;
}

std::vector<std::shared_ptr<sim::IRoutingDevice>> ReceiverMock::get_neighbours() const {
    return std::vector<std::shared_ptr<sim::IRoutingDevice>>();
};

std::shared_ptr<sim::ILink> ReceiverMock::next_inlink() { return nullptr; }

std::shared_ptr<sim::ILink> ReceiverMock::get_link_to_destination(
    std::shared_ptr<IRoutingDevice> device) const {
    return nullptr;
}

void ReceiverMock::process() {}
sim::DeviceType ReceiverMock::get_type() const {
    return sim::DeviceType::RECEIVER;
}

}  // namespace test
