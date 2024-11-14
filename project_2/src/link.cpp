#include "link.hpp"

#include <vector>
#include <cstdint>
#include <queue>
#include <memory>
#include <unordered_map>

Link::Link(std::shared_ptr<NetworkElement>& source, std::shared_ptr<NetworkElement>& destination, std::uint64_t speed, std::uint64_t last_process_time_ns):
    source(source),
    destination(destination),
    speed(speed),
    last_process_time_ns(last_process_time_ns)
    {}

std::shared_ptr<NetworkElement> Link::GetNextElement(std::uint64_t destination_id = 0) {
    return destination;
}

void Link::ReceivePacket(std::uint64_t current_time_ns, PacketHeader& packet, PriorityQueueWrapper& packets_wrapped) {
    std::uint64_t process_time = (packet.GetSize() + speed - 1) / speed;
    last_process_time_ns = std::max(last_process_time_ns, current_time_ns) + process_time;
    packet.SetSendingTime(last_process_time_ns);
    packets_wrapped.push(RoutingPacket(packet, GetNextElement()));
}
