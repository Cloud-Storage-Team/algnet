#include "Link.hpp"
#include "Device.hpp"
#include "Packet.hpp"
#include "Scheduler.hpp"
#include "event.hpp"

namespace sim {

Link::Link(Device* a_from, Device* a_to, unsigned int a_speed_mbps,
    unsigned int a_delay)
    : m_from(a_from), m_to(a_to), m_speed_mbps(a_speed_mbps), m_src_egress_delay(0), m_transmission_delay(a_delay) {}

void Link::schedule_arrival(Packet a_packet)
{

    unsigned int packet_size_bits = a_packet.size * 8;
    unsigned int transmission_time = (packet_size_bits + m_speed_mbps - 1) / m_speed_mbps;
    
    unsigned int total_delay = m_src_egress_delay + m_transmission_delay + transmission_time;
    
    
    m_src_egress_delay += transmission_time;
    m_next_ingress.push(a_packet);
    //The time parameter must be provided in the Arrive constructor.
    Scheduler::get_instance().add(Arrive(this, new Packet(a_packet)));


    //Important: m_src_egress_delay should be decreased after the Arrive event is processed,
    // not immediately after adding it to the Scheduler. This should be done inside Arrive::operator().

    /*
    void Arrive::operator()() {
        // Free up queue space after processing the packet
        link->m_src_egress_delay -= transmission_time;  

        // Forward the packet to the destination device
        link->m_to->process(*packet);
    }
    */
}

}