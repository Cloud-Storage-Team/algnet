#include "Receiver.hpp"
#include "NetworkSimulator.hpp"

#include <iostream>

Receiver::Receiver(double processing_delay_ns):
    NetworkDevice(DeviceType::Receiver, processing_delay_ns) {
    id = NetworkDevice::last_given_device_id++;
}

void Receiver::ProcessPacket(Packet p) {
    Enqueue(p);

    double packet_processing_delay_ns = processing_delay_ns;
    if (next_processing_time_ns > NetworkSimulator::Now()) {
        packet_processing_delay_ns += next_processing_time_ns - NetworkSimulator::Now();
    }

    if (!NetworkSimulator::EnableACK) {
        NetworkSimulator::Schedule(packet_processing_delay_ns, [this, packet_processing_delay_ns]() {
            Packet p = Dequeue();
            std::cout << "[INFO]: Received packet. Transmission time = " << (NetworkSimulator::Now() + packet_processing_delay_ns - p.m_sending_time_ns) << " ns.\n";
        });
        next_processing_time_ns = NetworkSimulator::Now() + packet_processing_delay_ns;
    }
}