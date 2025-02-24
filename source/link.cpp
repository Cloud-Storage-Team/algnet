#include "link.hpp"

namespace sim {

Packet Link::get_packet() {
    Packet result = m_next_ingress.front();
    m_next_ingress.pop();
    return result;
}

void Link::schedule_arrival(Packet a_packet) {}

}  // namespace sim
