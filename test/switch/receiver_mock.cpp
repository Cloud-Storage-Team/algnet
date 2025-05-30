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
                                        std::shared_ptr<sim::ILink> link,
                                        size_t paths_count) {
    return false;
}

std::shared_ptr<sim::ILink> ReceiverMock::next_inlink() { return nullptr; }

std::shared_ptr<sim::ILink> ReceiverMock::get_link_to_destination(
    std::shared_ptr<IRoutingDevice> device) const {
    return nullptr;
}

bool ReceiverMock::notify_about_arrival(Time arrival_time) {
    return false;
}

std::uint32_t ReceiverMock::process() { return 1; }
sim::DeviceType ReceiverMock::get_type() const {
    return sim::DeviceType::RECEIVER;
}

std::set<std::shared_ptr<sim::ILink>> ReceiverMock::get_outlinks() {
    return {};
}

Id ReceiverMock::get_id() const { return ""; }

}  // namespace test
