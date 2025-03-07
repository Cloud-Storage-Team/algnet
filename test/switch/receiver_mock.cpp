#include "receiver_mock.hpp"

namespace test {

void ReceiverMock::add_inlink(std::shared_ptr<sim::Link> link) {}

void ReceiverMock::update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                                        std::shared_ptr<sim::Link> link) {}

std::shared_ptr<sim::Link> ReceiverMock::next_inlink() { return nullptr; }

std::shared_ptr<sim::Link> ReceiverMock::get_destination(
    std::shared_ptr<IRoutingDevice> dest) const {
    return nullptr;
}

void ReceiverMock::process() {}
sim::DeviceType ReceiverMock::get_type() const {
    return sim::DeviceType::RECEIVER;
}

}  // namespace test
