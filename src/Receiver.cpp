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
    std::shared_ptr<NetworkDevice> prev_device = PrevDevice();

    /* Latency is a sum of processing and queuing delays */
    std::uint64_t latency = processing_delay_per_packet + std::max<std::uint64_t>(0, completion_time - NetworkSimulator::Now());

    std::uint64_t transmission_delay = 0;
    /* If ACKs are enabled, send ACK */
    if (NetworkSimulator::EnableACK) {
        std::uint64_t link_last_process_time = NetworkSimulator::GetLinkLastProcessTime(id, prev_device->id);
        /* Waiting for the link to process previous packets */
        transmission_delay = std::max<std::uint64_t>(0, link_last_process_time - (NetworkSimulator::Now() + latency)) +
                             NetworkSimulator::GetDistanceNs(id, prev_device->id);
        /* Update link last process time */
        NetworkSimulator::UpdateLinkLastProcessTime(id, prev_device->id, NetworkSimulator::Now() + latency + transmission_delay);
    }

    NetworkSimulator::Schedule(latency + transmission_delay, [this, prev_device]() {
        Packet p = Dequeue();
        std::cout << "[INFO]: Packet received. Transmission time = " << (NetworkSimulator::Now() - p.m_sending_time_ns) << " ns.\n";
        if (NetworkSimulator::EnableACK) {
            p.m_is_ack = true;
            prev_device->ProcessPacket(p);
        }
    });
    completion_time = NetworkSimulator::Now() + latency;
}