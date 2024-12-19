#include "Receiver.hpp"
#include "NetworkSimulator.hpp"

#include <iostream>

Receiver::Receiver(double processing_delay_ns):
        NetworkDevice(DeviceType::Receiver, processing_delay_ns) {
    id = NetworkDevice::last_given_device_id++;
}

void Receiver::ProcessPacket(Packet p) {
    Enqueue(p);
    std::shared_ptr<NetworkDevice> prev_device = PrevDevice();

    double latency = 0.0;
    /* Queueing delay */
    if (next_processing_time_ns > NetworkSimulator::Now()) {
        latency += next_processing_time_ns - NetworkSimulator::Now();
    }
    /* Processing delay */
    latency += processing_delay_ns;

    if (NetworkSimulator::EnableACK) {
        double link_last_process_time = NetworkSimulator::GetLinkLastProcessTime(id, prev_device->id);
        /* Waiting for the link to process previous packets */
        if (link_last_process_time > NetworkSimulator::Now()) {
            latency += link_last_process_time - NetworkSimulator::Now();
        }
        /* Transmission delay */
        latency += NetworkSimulator::GetDistanceNs(id, prev_device->id);
        /* Update link last process time */
        NetworkSimulator::UpdateLinkLastProcessTime(id, prev_device->id, NetworkSimulator::Now() + latency);
    }

    NetworkSimulator::Schedule(latency, [this, prev_device, latency]() {
        Packet p = Dequeue();
        double packet_delivery_time_ns = NetworkSimulator::Now() - NetworkSimulator::GetDistanceNs(id, prev_device->id);
        std::cout << "[INFO]: Packet received. Latency = " << (packet_delivery_time_ns - p.m_sending_time_ns) << " ns.\n";
        if (NetworkSimulator::EnableACK) {
            p.m_is_ack = true;
            prev_device->ProcessPacket(p);
        }
    });
    next_processing_time_ns = NetworkSimulator::Now() + latency;
}