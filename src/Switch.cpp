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

    std::uint64_t latency = 0;
    /* Queueing delay */
    if (next_processing_time_ns > NetworkSimulator::Now()) {
        latency += next_processing_time_ns - NetworkSimulator::Now();
    }
    /* Processing delay */
    latency += processing_delay_ns;

    std::uint64_t transmission_delay = 0;
    std::uint64_t link_last_process_time = NetworkSimulator::GetLinkLastProcessTime(id, next_device->id);
    /* Waiting for the link to process previous packets */
    if (link_last_process_time > NetworkSimulator::Now() + latency) {
        transmission_delay += link_last_process_time - NetworkSimulator::Now();
    }
    /* Transmission delay */
    transmission_delay += NetworkSimulator::GetDistanceNs(id, next_device->id);
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
    next_processing_time_ns = NetworkSimulator::Now() + latency;
}