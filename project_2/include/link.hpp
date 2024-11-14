#pragma once

#include "network_element.hpp"

#include <vector>
#include <cstdint>
#include <queue>
#include <memory>
#include <unordered_map>

// Network link abstraction
class Link : public NetworkElement {
protected:
    std::shared_ptr<NetworkElement> source;
    std::shared_ptr<NetworkElement> destination;
    // In bits per ns
    std::uint64_t speed;
    // Stores time when the last packet was pushed throw
    std::uint64_t last_process_time_ns;
    
public:
    Link() = default;
    Link(std::shared_ptr<NetworkElement>& source, std::shared_ptr<NetworkElement>& destination, std::uint64_t speed, std::uint64_t last_process_time_ns = 0);

    std::shared_ptr<NetworkElement> GetNextElement(std::uint64_t destination_id) override final;
    // Updates last_process_time_ns, sets new sending_time for packet and puts packet back to queue
    void ReceivePacket(std::uint64_t current_time_ns, PacketHeader& packet, PriorityQueueWrapper& packets_wrapped) override final;
    virtual ~Link() {}
};

struct Connection {
    std::shared_ptr<NetworkElement> direct_link;
    std::shared_ptr<NetworkElement> reversed_link;

    Connection(std::shared_ptr<Link>&& direct_link, std::shared_ptr<Link>&& reversed_link): 
        direct_link(std::move(direct_link)), 
        reversed_link(std::move(reversed_link)) {}
};