#include "Switch.hpp"
#include "NetworkSimulator.hpp"

Switch::Switch(std::uint64_t processing_delay_ns):
        NetworkDevice(processing_delay_ns) {
    id = NetworkDevice::last_given_device_id++;
}

void Switch::ProcessPacket(Packet p) {
    Enqueue(p);
    std::shared_ptr<Link> link;

    if (p.m_is_ack) {
        /* send ACK to packet sender */
        link = PrevLink(p.m_source_id);
    }
    else {
        /* send packet to receiver */
        link = NextLink(p.m_destination_id);
    }
    std::shared_ptr<NetworkDevice> next_device = link->destination;

    /* Latency is a sum of processing and queuing delays */
    std::uint64_t latency = processing_delay_per_packet + std::max<std::uint64_t>(0, completion_time - NetworkSimulator::Now());
    /* Waiting for the link to process previous packets, then add distance (i.e. link transmission time) */
    std::uint64_t transmission_delay = std::max<std::uint64_t>(0, link->last_process_time_ns - (NetworkSimulator::Now() + latency)) +
                                       link->distance_ns;
    /* Update link last process time */
    link->UpdateLastProcessTime(NetworkSimulator::Now() + latency + transmission_delay);

    NetworkSimulator::Schedule(latency + transmission_delay, [this, next_device]() {
        Packet p = Dequeue();
        next_device->ProcessPacket(p);
    });
    completion_time = NetworkSimulator::Now() + latency;
}