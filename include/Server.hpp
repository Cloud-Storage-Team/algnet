#pragma once

#include "NetworkDevice.hpp"
#include "Event.hpp"

#include <vector>
#include <queue>

/**
 * @brief
 */
class Server: public NetworkDevice {
public:
    Server();

    /**
     * @brief Sends events (i. e. packets) to the receiver
     *
     * @details Receiver -- next device in the flow path
     */
    void Send(std::shared_ptr<NetworkDevice> receiver, std::uint32_t distance_ns) override;

    /**
     * @brief Vector with packets sent by server
     */
    std::vector<Packet> sent_packets;

    /**
     * @brief Deque with incoming events (i. e. packets)
     */
    std::deque<Event> ingress_events;

    /**
     * @brief Overridden method to std::deque
     *
     * @return Event in the head of deque
     */
    const Event& TopStorage() override;

    /**
     * @brief Overridden method to std::deque
     *
     * @details Removes head event from deque
    */
    void PopStorage() override;

    /**
     * @brief Overridden method to std::deque
     *
     * @details Pushes event to the tail of deque
     */
    void PushStorage(const Event& event) override;
};