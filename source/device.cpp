#include "device.hpp"

namespace sim {

void RountingModule::add_inlink(Link* link) {
    m_inlinks.insert(link);
    m_next_inlink = m_inlinks.begin();
}

void RountingModule::update_routing_table(IRoutingDevice* dest, Link* link) {
    m_routing_table[dest] = link;
}

Link* RountingModule::next_inlink() {
    if (m_inlinks.empty()) {
        return nullptr;
    }

    Link* link = *m_next_inlink;
    if (++m_next_inlink == m_inlinks.end()) {
        m_next_inlink = m_inlinks.begin();
    }
    return link;
}

}  // namespace sim
