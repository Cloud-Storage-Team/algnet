#include "utils.hpp"

namespace test {

TestDevice::TestDevice() : sim::Device(sim::DeviceType::SENDER) {}

void TestDevice::process() {}

bool TestDevice::has_inlink(sim::Link* link) {
    return m_inlinks.find(link) != m_inlinks.end();
}

bool TestDevice::check_route(sim::Device* device, sim::Link* link) {
    if (m_routing_table.find(device) == m_routing_table.end()) {
        return false;
    }
    return m_routing_table.at(device) == link;
}

TestLink::TestLink(sim::Device* a_src, sim::Device* a_dest)
    : Link(a_src, a_dest, 0, 0) {}

}  // namespace sim
