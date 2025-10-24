#include "utils.hpp"

namespace test {

bool TestDevice::notify_about_arrival([[maybe_unused]] TimeNs arrival_time) {
    return false;
};

TimeNs TestDevice::process() { return TimeNs(0); };

std::vector<std::shared_ptr<sim::IDevice>> createTestDevices(size_t count) {
    std::vector<std::shared_ptr<sim::IDevice>> devices;
    for (size_t i = 0; i < count; ++i) {
        devices.emplace_back(std::make_shared<TestDevice>());
    }
    return devices;
}

TestLink::TestLink(std::shared_ptr<sim::IDevice> a_src,
                   std::shared_ptr<sim::IDevice> a_dest,
                   sim::Packet packet_to_return)
    : src(a_src), dst(a_dest), packet(packet_to_return) {}

void TestLink::schedule_arrival([[maybe_unused]] sim::Packet packet) {};

std::optional<sim::Packet> TestLink::get_packet() { return {packet}; };

std::shared_ptr<sim::IDevice> TestLink::get_from() const { return src.lock(); };
std::shared_ptr<sim::IDevice> TestLink::get_to() const { return dst.lock(); };

SizeByte TestLink::get_from_egress_queue_size() const { return SizeByte(0); }
SizeByte TestLink::get_max_from_egress_buffer_size() const {
    return SizeByte(4096);
}

SizeByte TestLink::get_to_ingress_queue_size() const { return SizeByte(0); }
SizeByte TestLink::get_max_to_ingress_queue_size() const {
    return SizeByte(4096);
}

Id TestLink::get_id() const { return ""; }

}  // namespace test
