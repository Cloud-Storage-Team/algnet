#include "Receiver.hpp"
#include "NetworkSimulator.hpp"

#include <iostream>

Receiver::Receiver(std::uint64_t processing_delay_ns):
    NetworkDevice(processing_delay_ns) {
    id = NetworkDevice::last_given_device_id++;
}

void Receiver::ProcessPacket(Packet p) {
    Enqueue(p);
    std::shared_ptr<NetworkDevice> prev_device = PrevDevice();

    std::uint64_t latency = 0;
    /* Queueing delay */
    if (next_processing_time_ns > NetworkSimulator::Now()) {
        latency += next_processing_time_ns - NetworkSimulator::Now();
    }
    /* Processing delay */
    latency += processing_delay_ns;

    std::uint64_t transmission_delay = 0;
    if (NetworkSimulator::EnableACK) {
        std::uint64_t link_last_process_time = NetworkSimulator::GetLinkLastProcessTime(id, prev_device->id);
        /* Waiting for the link to process previous packets */
        if (link_last_process_time > NetworkSimulator::Now() + latency) {
            transmission_delay += link_last_process_time - NetworkSimulator::Now();
        }
        /* Transmission delay */
        transmission_delay += NetworkSimulator::GetDistanceNs(id, prev_device->id);
        /* Update link last process time */
        NetworkSimulator::UpdateLinkLastProcessTime(id, prev_device->id, NetworkSimulator::Now() + latency + transmission_delay);
    }

    NetworkSimulator::Schedule(latency + transmission_delay, [this, prev_device, transmission_delay]() {
        Packet p = Dequeue();
        std::uint64_t packet_delivery_time_ns = NetworkSimulator::Now() - transmission_delay;
        std::cout << "[INFO]: Packet received. Latency = " << (packet_delivery_time_ns - p.m_sending_time_ns) << " ns.\n";
        if (NetworkSimulator::EnableACK) {
            p.m_is_ack = true;
            prev_device->ProcessPacket(p);
        }
    });
    next_processing_time_ns = NetworkSimulator::Now() + latency;
}