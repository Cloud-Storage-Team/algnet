#include "Receiver.hpp"
#include "NetworkSimulator.hpp"

#include <iostream>

Receiver::Receiver(std::uint64_t processing_delay_ns):
    NetworkDevice(processing_delay_ns) {
    id = NetworkDevice::last_given_device_id++;
}

void Receiver::ProcessPacket(Packet p) {
    Enqueue(p);
    /* Previous device in the flow path */
    std::shared_ptr<Link> link = NextLink(p.m_source_id);
    std::shared_ptr<NetworkDevice> next_device = link->destination;

    /* Latency is a sum of processing and queuing delays */
    std::uint64_t latency = processing_delay_per_packet + std::max<std::uint64_t>(0, completion_time - NetworkSimulator::Now());

    std::uint64_t transmission_delay = 0;
    /* If ACKs are enabled, send ACK */
    if (NetworkSimulator::EnableACK) {
        /* Waiting for the link to process previous packets */
        transmission_delay = (std::uint64_t)std::max<std::uint64_t>(0, (std::int64_t)(link->last_process_time_ns - (NetworkSimulator::Now() + latency))) +
                             link->distance_ns;
        /* Update link last process time */
        link->UpdateLastProcessTime(NetworkSimulator::Now() + latency + transmission_delay);
    }

    NetworkSimulator::Schedule(latency + transmission_delay, [this, next_device]() {
        Packet p = Dequeue();
        if (NetworkSimulator::EnableACK) {
            p.m_is_ack = true;
            next_device->ProcessPacket(p);
        }
    });
    completion_time = NetworkSimulator::Now() + latency;
}