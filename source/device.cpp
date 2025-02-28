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
