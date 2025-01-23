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

std::shared_ptr<NetworkElement> Link::GetNextElement(std::uint64_t destination_id) {
    return destination;
}

void Link::ReceivePacket(std::uint64_t current_time_ns, PacketHeader& packet, std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, EventComparator>& all_events) {
    std::uint64_t process_time = (packet.size + speed - 1) / speed;
    last_process_time_ns = std::max(last_process_time_ns, current_time_ns) + process_time;
    packet.sending_time = last_process_time_ns;
    auto last_process_time = last_process_time_ns;
    auto process = [this, packet, last_process_time](std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, EventComparator>& events) {
            PacketHeader inner_packet = packet;
            this->GetNextElement()->ReceivePacket(last_process_time, inner_packet, events);
        };
    all_events.push(std::make_shared<Event>(Event(last_process_time_ns, process, packet.destination_id)));
}
