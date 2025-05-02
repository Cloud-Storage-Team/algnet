#include "utils.hpp"

namespace test {

std::vector<std::shared_ptr<sim::IRoutingDevice>> createRoutingModules(
    size_t count) {
    std::vector<std::shared_ptr<sim::IRoutingDevice>> modules;
    for (size_t i = 0; i < count; ++i) {
        modules.emplace_back(std::make_shared<sim::RoutingModule>());
    }
    return modules;
}

TestLink::TestLink(std::shared_ptr<sim::IRoutingDevice> a_src,
                   std::shared_ptr<sim::IRoutingDevice> a_dest,
                   sim::Packet packet_to_return)
    : src(a_src), dst(a_dest), packet(packet_to_return) {}

void TestLink::schedule_arrival(Time current_time, sim::Packet packet) {};
void TestLink::process_arrival(sim::Packet packet) {};

std::optional<sim::Packet> TestLink::get_packet() { return {packet}; };

std::shared_ptr<sim::IRoutingDevice> TestLink::get_from() const { return src.lock(); };
std::shared_ptr<sim::IRoutingDevice> TestLink::get_to() const { return dst.lock(); };

Id TestLink::get_id() const { return 42; }

}  // namespace test
