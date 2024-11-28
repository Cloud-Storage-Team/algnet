#pragma once

#include <memory>
#include <unordered_map>

#include "routing_packet.hpp"
#include "utils.hpp"

class PriorityQueueWrapper;

// Abstract network element (link, switch, host, etc.)
class NetworkElement {
  public:
    NetworkElement() = default;

    // Return next element in rout
    virtual std::shared_ptr<NetworkElement>
    GetNextElement(std::uint64_t destination_id) = 0;
    /**
     * Processes the obtained packet (basicly adds processing time to
     * packet's sending time)
     *
     * @param current_time_ns time of obtaining packet on element
     * @param packets_wrapped required for returning updated packets back to
     * simulation queue
     */
    virtual void ReceivePacket(std::uint64_t current_time_ns,
                               PacketHeader &packet,
                               PriorityQueueWrapper &packets_wrapped) = 0;
    virtual ~NetworkElement() {
    }
};

// Abstract network element that has routing table (switch, host, etc.)
class RoutingNetworkElement : public NetworkElement {
  protected:
    std::unordered_map<std::uint64_t, std::shared_ptr<NetworkElement>>
        routing_table{};

  public:
    RoutingNetworkElement() = default;
    RoutingNetworkElement(
        std::unordered_map<std::uint64_t, std::shared_ptr<NetworkElement>>
            &routing_table);
    void AddNewRout(std::uint64_t destination_id,
                    std::shared_ptr<NetworkElement> &link);

    // Return next element in rout
    std::shared_ptr<NetworkElement>
    GetNextElement(std::uint64_t destination_id) override final;
    /**
     * Processes the obtained packet (basicly adds processing time to
     * packet's sending time)
     *
     * @param current_time_ns time of obtaining packet on element
     * @param packets_wrapped required for returning updated packets back to
     * simulation queue
     */
    virtual void ReceivePacket(std::uint64_t current_time_ns,
                               PacketHeader &packet,
                               PriorityQueueWrapper &packets_wrapped) = 0;
};