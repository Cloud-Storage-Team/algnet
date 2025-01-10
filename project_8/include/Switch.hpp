#pragma once

#include "NetworkDevice.hpp"
#include "Packet.hpp"
#include "Link.hpp"

#include <queue>
#include <unordered_map>
#include <memory>
#include <cstdint>
#include <vector>

class Switch : public NetworkDevice 
{
public:
    explicit Switch(std::uint64_t processing_delay_ns);

    // Receive and process a packet
    void ProcessPacket(Packet p) override;

    // Update the routing table
    void UpdateRoutingTable(std::uint64_t destination_id, std::vector<std::shared_ptr<Link>> links);

    // Method to process the packet queue
    void ProcessQueue();

private:
    // Queue of packets
    std::queue<Packet> packet_queue_;
    // Time of the last process
    std::uint64_t last_process_time_ns_ = 0;
};
