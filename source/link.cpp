#include "link.hpp"

namespace sim {

Link::Link(std::weak_ptr<IRoutingDevice>  a_from, std::weak_ptr<IRoutingDevice>  a_to,
         std::uint32_t a_speed_mbps, std::uint32_t a_delay):
    m_from(a_from),
    m_to(a_to),
    m_speed_mbps(a_speed_mbps),
    m_src_egress_delay(a_delay),
    m_transmission_delay(0),
    m_next_ingress()
{}
}
