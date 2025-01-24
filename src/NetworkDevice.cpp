#include "NetworkDevice.hpp"
#include "NetworkSimulator.hpp"

NetworkDevice::NetworkDevice(std::uint64_t processing_delay_ns):
    processing_delay_per_packet(processing_delay_ns) { }

void NetworkDevice::Enqueue(Packet p) {
    buffer.push(p);
}

Packet NetworkDevice::Dequeue() {
    Packet result = buffer.front();
    buffer.pop();
    return result;
}

bool NetworkDevice::Empty() const {
    return buffer.empty();
}

std::shared_ptr<Link> NetworkDevice::NextLink(std::uint32_t dest_id) const {
    return routing_table.at(dest_id);
}
