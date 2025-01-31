#include "Sender.hpp"
#include "NetworkSimulator.hpp"

#include <iostream>

Sender::Sender(std::uint64_t processing_delay_ns):
    NetworkDevice(processing_delay_ns) { }

void Sender::ProcessPacket(Packet p) {
    Enqueue(p);
    /* Next device in the flow path */
    std::shared_ptr<Link> link = NextLink(p.destination_id);

    /* Latency is a sum of processing and queuing delays */
    std::uint64_t latency = processing_delay_per_packet + std::max<std::uint64_t>(0, completion_time - NetworkSimulator::Now());

    std::uint64_t transmission_delay = 0;
    /* If ACK flag set to false, send the packet to the next device */
    if (!p.is_ack) {
        /* Waiting for the link to process previous packets, then add distance (i.e. link transmission time) */
        transmission_delay = (std::uint64_t)std::max<std::int64_t>(0, (std::int64_t)(link->last_processing_time_ns - (NetworkSimulator::Now() + latency))) +
                             link->delay_ns;
        /* Update link last process time */
        link->UpdateLastProcessTime(NetworkSimulator::Now() + latency + transmission_delay);
    }
    std::shared_ptr<NetworkDevice> next_device = link->dest;

    NetworkSimulator::Schedule(latency + transmission_delay, [this, next_device, transmission_delay]() {
        Packet p = Dequeue();
        if (p.is_ack) {
            NetworkSimulator::logger.Log(Metric::RTT, NetworkSimulator::Now(), NetworkSimulator::Now() - p.sending_time_ns);
        }
        else {
            /* Packet sending time*/
            p.sending_time_ns = NetworkSimulator::Now() - transmission_delay;
            next_device->ProcessPacket(p);
        }
    });
    completion_time = NetworkSimulator::Now() + latency;
}
