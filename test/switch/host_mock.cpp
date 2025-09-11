#include "host_mock.hpp"

namespace test {

bool HostMock::add_inlink([[maybe_unused]] std::shared_ptr<sim::ILink> link) {
    return false;
}

bool HostMock::add_outlink([[maybe_unused]] std::shared_ptr<sim::ILink> link) {
    return false;
}

bool HostMock::update_routing_table(
    [[maybe_unused]] Id dest_id,
    [[maybe_unused]] std::shared_ptr<sim::ILink> link,
    [[maybe_unused]] size_t paths_count) {
    return false;
}

std::shared_ptr<sim::ILink> HostMock::next_inlink() { return nullptr; }

std::shared_ptr<sim::ILink> HostMock::get_link_to_destination(
    [[maybe_unused]] sim::Packet packet) const {
    return nullptr;
}

bool HostMock::notify_about_arrival([[maybe_unused]] TimeNs arrival_time) {
    return false;
}

TimeNs HostMock::process() { return TimeNs(1); }

std::set<std::shared_ptr<sim::ILink>> HostMock::get_outlinks() { return {}; }

Id HostMock::get_id() const { return ""; }

void HostMock::enqueue_packet([[maybe_unused]] sim::Packet packet) { return; }

TimeNs HostMock::send_packet() { return TimeNs(0); }

}  // namespace test
