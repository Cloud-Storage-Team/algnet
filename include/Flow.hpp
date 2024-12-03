#pragma once

#include "Packet.hpp"
#include "NetworkDevice.hpp"

#include <cstdint>
#include <memory>

/**
 * Packet flow between source and destination nodes (network devices).
 */
class Flow {
public:
    explicit Flow(const NetworkDevice& src, const NetworkDevice& dst);

    /**
     * @brief Send packets from every server sender in the amount of CWND size.
     */
    void Send();

    /**
     * @brief Pointer to flow's source node
     */
    std::shared_ptr<NetworkDevice> source_node;

    /**
     * @brief Pointer to flow's destination node
     */
    std::shared_ptr<NetworkDevice> destination_node;
};
