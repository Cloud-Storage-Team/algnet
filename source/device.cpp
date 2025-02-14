#include "device.hpp"

namespace sim {

void Device::add_inlink(Link* link) {
    m_inlinks.insert(link);

    // TODO: maybe some update of m_next_inlink is needed here
}

void Device::add_outlink(Link* link, Device* dest) {
    m_outlinks[dest] = link;
}


}  // namespace sim
