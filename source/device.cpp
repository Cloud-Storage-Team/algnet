#include "device.hpp"

namespace sim {

void RountingModule::add_inlink(std::shared_ptr<Link> link) {
    m_inlinks.insert(link);
    m_next_inlink = m_inlinks.begin();
}

void RountingModule::update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                                          std::shared_ptr<Link> link) {
    m_routing_table[dest] = link;
}

std::shared_ptr<Link> RountingModule::next_inlink() {
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
