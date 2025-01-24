#pragma once

#include "link.hpp"
#include "packet.hpp"
#include "event.hpp"

#include <unordered_map>
#include <queue>
#include <memory>

class Link;

class NetworkElement {
protected:
    std::unordered_map<std::uint64_t, std::shared_ptr<Link>> routing_table{};
public:
    NetworkElement() = default;
    NetworkElement(const std::unordered_map<std::uint64_t, std::shared_ptr<Link>>& routing_table);

    void AddNewRoute(std::uint64_t destination_id, std::shared_ptr<Link>& link);

    virtual std::shared_ptr<Link> GetNextElement(std::uint64_t destination_id);
    virtual void ReceivePacket(std::uint64_t current_time_ns, PacketHeader& packet, std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, EventComparator>& all_events) = 0;
};