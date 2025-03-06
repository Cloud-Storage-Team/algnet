#include "device.hpp"

#include "link.hpp"

namespace sim {

void RoutingModule::add_inlink(std::shared_ptr<Link> link) {
    m_inlinks.insert(link);
    m_next_inlink = m_inlinks.begin();
}

void RoutingModule::update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                                         std::shared_ptr<Link> link) {
    m_routing_table[dest] = link;
}

std::shared_ptr<Link> RoutingModule::next_inlink() {
    if (m_inlinks.empty()) {
        return nullptr;
    }

    std::shared_ptr<Link> link = *m_next_inlink;
    if (++m_next_inlink == m_inlinks.end()) {
        m_next_inlink = m_inlinks.begin();
    }
    return link;
}

}  // namespace sim
