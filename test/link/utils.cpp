#include "utils.hpp"

#include <memory>

namespace test {

void LinkTest::SetUp() {}

void LinkTest::TearDown() { sim::Scheduler::get_instance().clear(); }

Id DeviceMock::get_id() const { return ""; };

bool DeviceMock::add_inlink(std::shared_ptr<sim::ILink> link) { return false; }

bool DeviceMock::add_outlink(std::shared_ptr<sim::ILink> link) { return false; }

bool DeviceMock::update_routing_table(Id dest_id,
                                      std::shared_ptr<sim::ILink> link,
                                      size_t paths_count) {
    return false;
}

std::set<std::shared_ptr<sim::ILink>> DeviceMock::get_outlinks() { return {}; }

std::shared_ptr<sim::ILink> DeviceMock::get_link_to_destination(
    sim::Packet packet) const {
    return nullptr;
}

std::shared_ptr<sim::ILink> DeviceMock::next_inlink() { return {}; }

bool DeviceMock::notify_about_arrival(Time arrival_time) { return false; };

}  // namespace test
