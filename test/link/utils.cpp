#include "utils.hpp"

#include "scheduler.hpp"

namespace test {

void LinkTest::SetUp() {};

void LinkTest::TearDown() { sim::Scheduler::get_instance().clear(); };

void DeviceMock::add_inlink(std::shared_ptr<sim::ILink> link) {

};

void DeviceMock::update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                                      std::shared_ptr<sim::ILink> link) {

};

std::shared_ptr<sim::ILink> DeviceMock::next_inlink() { return nullptr; };

}  // namespace test
