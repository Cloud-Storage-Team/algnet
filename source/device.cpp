<<<<<<< HEAD
#include "device.hpp"

namespace sim {

void Device::add_inlink(Link* link) {
    m_inlinks.insert(link);

    // TODO: maybe some update of m_next_inlink is needed here
}

void Device::update_routing_table(Device* dest, Link* link) {
    m_routing_table[dest] = link;
}

}  // namespace sim
=======
#include "device.hpp"

namespace sim {

void RoutingModule::add_inlink(std::shared_ptr<ILink> link) {
    m_inlinks.insert(link);
    m_next_inlink = m_inlinks.begin();
}

void RoutingModule::update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                                         std::shared_ptr<ILink> link) {
    m_routing_table[dest] = link;
}

std::shared_ptr<ILink> RoutingModule::next_inlink() {
    if (m_inlinks.empty()) {
        return nullptr;
    }

    std::shared_ptr<ILink> link = *m_next_inlink;
    if (++m_next_inlink == m_inlinks.end()) {
        m_next_inlink = m_inlinks.begin();
    }
    return link;
}

}  // namespace sim
>>>>>>> origin/main
