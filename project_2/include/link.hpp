#pragma once
/**
 * @file link.hpp
 * @author your name (you@domain.com)
 * @brief Contains class that describes network link and class that
 * @date 2024-11-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "network_element.hpp"

#include <vector>
#include <cstdint>
#include <queue>
#include <memory>
#include <unordered_map>

/**
 * @brief Network link abstraction
 * 
 */
class Link : public NetworkElement {
protected:
    std::shared_ptr<NetworkElement> source;
    std::shared_ptr<NetworkElement> destination;
    // In bits per ns
    std::uint64_t speed;
    // Stores time when the last packet was pushed through
    std::uint64_t last_process_time_ns;
    
public:
    Link() = default;
    /**
     * @brief Construct a new Link object
     * 
     * @param source network element which sends packets
     * @param destination network element which receives packets
     * @param speed the speed of pushing packets into the link
     * @param last_process_time_ns time, when last packet was sent to destination
     */
    Link(std::shared_ptr<NetworkElement>& source, std::shared_ptr<NetworkElement>& destination, std::uint64_t speed, std::uint64_t last_process_time_ns = 0);

    /**
     * @brief Returns receiver of packets passed by link
     * 
     * @param destination_id packet destination id (is not used by link, because it does not have routing table)
     * @return std::shared_ptr<NetworkElement> packets receiver connected to link
     */
    std::shared_ptr<NetworkElement> GetNextElement(std::uint64_t destination_id = 0) override final;
    /**
     * @brief Updates last_process_time_ns, sets new sending_time for packet and puts packet back to queue
     * 
     * @param current_time_ns time of receiving packet by link
     * @param packet packet, received by link
     * @param packets_wrapped queue of packets, where packet should be pushed after processing
     */
    void ReceivePacket(std::uint64_t current_time_ns, PacketHeader& packet, std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, EventComparator>& all_events) override final;
    virtual ~Link() {}
};

struct Connection {
    std::shared_ptr<NetworkElement> direct_link;
    std::shared_ptr<NetworkElement> reversed_link;

    Connection(std::shared_ptr<Link>&& direct_link, std::shared_ptr<Link>&& reversed_link): 
        direct_link(std::move(direct_link)), 
        reversed_link(std::move(reversed_link)) {}
};