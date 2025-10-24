#include "utils.hpp"

#include <memory>

namespace test {

void LinkTest::SetUp() {}

void LinkTest::TearDown() { sim::Scheduler::get_instance().clear(); }

Id DeviceMock::get_id() const { return ""; };

bool DeviceMock::add_inlink([[maybe_unused]] std::shared_ptr<sim::ILink> link) {
    return false;
}

bool DeviceMock::add_outlink(
    [[maybe_unused]] std::shared_ptr<sim::ILink> link) {
    return false;
}

bool DeviceMock::update_routing_table(
    [[maybe_unused]] Id dest_id,
    [[maybe_unused]] std::shared_ptr<sim::ILink> link,
    [[maybe_unused]] size_t paths_count) {
    return false;
}

std::set<std::shared_ptr<sim::ILink>> DeviceMock::get_outlinks() { return {}; }

std::shared_ptr<sim::ILink> DeviceMock::get_link_to_destination(
    [[maybe_unused]] sim::Packet packet) const {
    return nullptr;
}

std::shared_ptr<sim::ILink> DeviceMock::next_inlink() { return {}; }

bool DeviceMock::notify_about_arrival([[maybe_unused]] TimeNs arrival_time) {
    return false;
};

TimeNs DeviceMock::process() { return TimeNs(0); };

}  // namespace test
