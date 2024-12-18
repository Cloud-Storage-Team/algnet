#include "Sender.hpp"
#include "NetworkSimulator.hpp"

Sender::Sender(double processing_delay_ns):
    NetworkDevice(DeviceType::Sender, processing_delay_ns) {
    id = NetworkDevice::last_given_device_id++;
}

void Sender::ProcessPacket(Packet p) {
    Enqueue(p);
    std::shared_ptr<NetworkDevice> next_device = NextDevice();

    double packet_processing_delay_ns = processing_delay_ns;
    if (next_processing_time_ns > NetworkSimulator::Now()) {
        packet_processing_delay_ns += next_processing_time_ns - NetworkSimulator::Now();
    }

    NetworkSimulator::Schedule(packet_processing_delay_ns, [this, next_device]() {
        Packet p = Dequeue();
        p.m_sending_time_ns = NetworkSimulator::Now();
        next_device->ProcessPacket(p);
    });
    next_processing_time_ns = NetworkSimulator::Now() + packet_processing_delay_ns;
}