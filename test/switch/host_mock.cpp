#include "host_mock.hpp"

namespace test {

bool HostMock::add_inlink(std::shared_ptr<sim::ILink> link) { return false; }

bool HostMock::add_outlink(std::shared_ptr<sim::ILink> link) { return false; }

bool HostMock::update_routing_table(Id dest_id,
                                    std::shared_ptr<sim::ILink> link,
                                    size_t paths_count) {
    return false;
}

std::shared_ptr<sim::ILink> HostMock::next_inlink() { return nullptr; }

std::shared_ptr<sim::ILink> HostMock::get_link_to_destination(
    sim::Packet packet) const {
    return nullptr;
}

bool HostMock::notify_about_arrival(TimeNs arrival_time) { return false; }

TimeNs HostMock::process() { return TimeNs(1); }
sim::DeviceType HostMock::get_type() const { return sim::DeviceType::RECEIVER; }

std::set<std::shared_ptr<sim::ILink>> HostMock::get_outlinks() { return {}; }

Id HostMock::get_id() const { return ""; }

void HostMock::enqueue_packet(sim::Packet packet) { return; }

TimeNs HostMock::send_packet() { return TimeNs(0); }

}  // namespace test
