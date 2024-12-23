#include "Switch.hpp"
#include "NetworkSimulator.hpp"

Switch::Switch(std::uint64_t processing_delay_ns):
    NetworkDevice(processing_delay_ns) {
    id = NetworkDevice::last_given_device_id++;
}

void Switch::ProcessPacket(Packet p) {
    Enqueue(p);
    std::shared_ptr<NetworkDevice> next_device;

    if (p.m_is_ack) {
        next_device = PrevDevice();
    }
    else {
        next_device = NextDevice();
    }
    /* Latency is a sum of processing and queuing delays */
    std::uint64_t latency = processing_delay_per_packet + std::max<std::uint64_t>(0, completion_time - NetworkSimulator::Now());
    std::uint64_t link_last_process_time = NetworkSimulator::GetLinkLastProcessTime(id, next_device->id);
    /* Waiting for the link to process previous packets, then add distance (i.e. link transmission time) */
    std::uint64_t transmission_delay = std::max<std::uint64_t>(0, link_last_process_time - (NetworkSimulator::Now() + latency)) +
                                       NetworkSimulator::GetDistanceNs(id, next_device->id);
    /* Update link last process time */
    NetworkSimulator::UpdateLinkLastProcessTime(id, next_device->id, NetworkSimulator::Now() + latency + transmission_delay);

    NetworkSimulator::Schedule(latency + transmission_delay, [this]() {
        Packet p = Dequeue();
        if (p.m_is_ack) {
            std::shared_ptr<NetworkDevice> prev_device = PrevDevice();
            prev_device->ProcessPacket(p);
        }
        else {
            std::shared_ptr<NetworkDevice> next_device = NextDevice();
            next_device->ProcessPacket(p);
        }
    });
    completion_time = NetworkSimulator::Now() + latency;
}