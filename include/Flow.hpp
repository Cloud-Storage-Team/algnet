#pragma once

#include "Packet.hpp"
#include "NetworkDevice.hpp"

#include <cstdint>
#include <memory>

/**
 * @brief Network flow between the source and destination nodes
 */
class Flow {
public:
    explicit Flow(const NetworkDevice& src, const NetworkDevice& dst);

    /**
     * @brief Pointer to flow's source node
     */
    std::shared_ptr<NetworkDevice> source_node;

    /**
     * @brief Pointer to flow's destination node
     */
    std::shared_ptr<NetworkDevice> destination_node;
};
