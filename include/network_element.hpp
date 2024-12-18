#pragma once

#include "packet.hpp"
#include "event.hpp"

#include <unordered_map>
#include <queue>
#include <memory>

class RoutingPacket;

/**
 * @brief Abstract network element (link, switch, host, etc.)
 * 
 */
class NetworkElement {
public:
    NetworkElement() = default;
    
    /**
     * @brief get next element in route
     * 
     */
    virtual std::shared_ptr<NetworkElement> GetNextElement(std::uint64_t destination_id) = 0;

    /**
     * @brief Processes the obtained packet (basically adds processing time to packet's sending time)
     * 
     */
    virtual void ReceivePacket(std::uint64_t current_time_ns, PacketHeader& packet, std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, EventComparator>& all_events) = 0;
    virtual ~NetworkElement() {}
};

/**
 * @brief Abstract network element that has routing table (switch, host, etc.)
 * 
 */
class RoutingNetworkElement: public NetworkElement {
protected:
    std::unordered_map<std::uint64_t, std::shared_ptr<NetworkElement>> routing_table{};
public:
    RoutingNetworkElement() = default;
    /**
     * @brief Construct a new Routing Network Element
     * 
     */
    RoutingNetworkElement(const std::unordered_map<std::uint64_t, std::shared_ptr<NetworkElement>>& routing_table);

    /**
     * @brief Creates new record in routing table
     * 
     */
    void AddNewRoute(std::uint64_t destination_id, std::shared_ptr<NetworkElement>& link);


    /**
     * @brief get next element in route
     * 
     */
    std::shared_ptr<NetworkElement> GetNextElement(std::uint64_t destination_id) override final;
    virtual void ReceivePacket(std::uint64_t current_time_ns, PacketHeader& packet, std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, EventComparator>& all_events) = 0;
};