#pragma once

#include "Packet.hpp"

#include <memory>
#include <cstdint>

class Event {
public:
    explicit Event(std::shared_ptr<Packet> packet, std::uint32_t flow_id, std::uint32_t receiver_id,
                   std::uint64_t delivery_time_ns, bool is_ACK);

    /**
     * @brief Pointer to the packet
     */
    std::shared_ptr<Packet> packet;

    /**
     * @brief ID of network device -- receiver of the event
     */
    std::uint32_t receiver_id;

    /**
     * @brief ID of the corresponding flow
     */
    std::uint32_t flow_id;

    /**
     * @brief Packet delivery time in nanoseconds
     */
    std::uint64_t delivery_time_ns;

    /**
     * @brief Acknowledgement flag
     *
     * @details True: ACK
     * @details False: Packet
     */
    bool is_ACK;

    bool operator<(const Event& other) const;
};