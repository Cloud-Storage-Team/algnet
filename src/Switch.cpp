#include "Switch.hpp"
#include "NetworkSimulator.hpp"

Switch::Switch(std::uint64_t processing_delay_ns):
    NetworkDevice(processing_delay_ns) { }

void Switch::ProcessPacket(Packet p) {
    Enqueue(p);
    std::shared_ptr<Link> link;

    if (p.is_ack) {
        /* send ACK to packet sender */
        link = NextLink(p.source_id);
    }
    else {
        /* send packet to receiver */
        link = NextLink(p.destination_id);
    }
    std::shared_ptr<NetworkDevice> next_device = link->dest;

    /* Latency is a sum of processing and queuing delays */
    std::uint64_t latency = processing_delay_per_packet + std::max<std::uint64_t>(0, completion_time - NetworkSimulator::Now());
    /* Waiting for the link to process previous packets, then add distance (i.e. link transmission time) */
    std::uint64_t transmission_delay = (std::uint64_t)std::max<std::uint64_t>(0, (std::int64_t)(link->last_processing_time_ns - (NetworkSimulator::Now() + latency))) +
                                       link->delay_ns;
    /* Update link last process time */
    link->UpdateLastProcessTime(NetworkSimulator::Now() + latency + transmission_delay);

    NetworkSimulator::Schedule(latency + transmission_delay, [this, next_device]() {
        Packet p = Dequeue();
        NetworkSimulator::logger.Log(Metric::QUEUE_SIZE, NetworkSimulator::Now(), buffer.size());
        next_device->ProcessPacket(p);
    });
    completion_time = NetworkSimulator::Now() + latency;
}
