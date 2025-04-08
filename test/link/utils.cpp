#include "utils.hpp"

#include "scheduler.hpp"

namespace test {

void LinkTest::SetUp() {}

void LinkTest::TearDown() { sim::Scheduler::get_instance().clear(); }

bool DeviceMock::add_inlink(std::shared_ptr<sim::ILink> link) { return false; }

bool DeviceMock::add_outlink(std::shared_ptr<sim::ILink> link) { return false; }

bool DeviceMock::update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                                      std::shared_ptr<sim::ILink> link) {
    return false;
}

std::set<std::shared_ptr<sim::ILink>> DeviceMock::get_outlinks() const {
    return {};
}

std::vector<std::shared_ptr<sim::IRoutingDevice>> DeviceMock::get_neighbours()
    const {
    return std::vector<std::shared_ptr<sim::IRoutingDevice>>();
}

std::shared_ptr<sim::ILink> DeviceMock::get_link_to_destination(
    std::shared_ptr<IRoutingDevice> device) const {
    return nullptr;
}

std::shared_ptr<sim::ILink> DeviceMock::next_inlink() { return nullptr; };

}  // namespace test
