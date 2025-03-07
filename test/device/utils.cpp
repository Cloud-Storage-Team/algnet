#include "utils.hpp"

namespace test {

TestRoutingModule::TestRoutingModule() {}

bool TestRoutingModule::has_inlink(std::shared_ptr<sim::Link> link) {
    return m_inlinks.find(link) != m_inlinks.end();
}

bool TestRoutingModule::check_route(std::shared_ptr<sim::IRoutingDevice> device,
                                    std::shared_ptr<sim::Link> link) {
    if (m_routing_table.find(device) == m_routing_table.end()) {
        return false;
    }
    return m_routing_table.at(device) == link;
}

TestLink::TestLink(std::shared_ptr<sim::IRoutingDevice> a_src,
                   std::shared_ptr<sim::IRoutingDevice> a_dest)
    : sim::Link(a_src, a_dest, 0, 0) {}

}  // namespace test
