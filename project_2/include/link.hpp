#pragma once
#include "event.hpp"
#include "packet.hpp"

#include <vector>
#include <cstdint>
#include <queue>
#include <memory>
#include <unordered_map>

class NetworkElement;
class Event;
struct EventComparator;

class Link {
protected:
    std::shared_ptr<NetworkElement> source;
    std::shared_ptr<NetworkElement> destination;
    // In bits per ns
    std::uint64_t speed;
    // Stores time when the last packet was pushed through
    std::uint64_t last_process_time_ns;
    
public:
    Link() = default;

    Link(std::shared_ptr<NetworkElement>& source, std::shared_ptr<NetworkElement>& destination, std::uint64_t speed, std::uint64_t last_process_time_ns = 0);
    std::shared_ptr<NetworkElement> GetNextElement(std::uint64_t destination_id = 0);
    void ReceivePacket(std::uint64_t current_time_ns, PacketHeader& packet, EventQueue& all_events);
    void ProcessPacket(std::uint64_t current_time_ns, EventQueue& all_events);
    virtual ~Link() {}
};

struct Connection {
    std::shared_ptr<Link> direct_link;
    std::shared_ptr<Link> reversed_link;

    Connection(std::shared_ptr<Link>&& direct_link, std::shared_ptr<Link>&& reversed_link): 
        direct_link(std::move(direct_link)), 
        reversed_link(std::move(reversed_link)) {}
};