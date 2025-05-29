#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <queue>

#include "utils/identifier_factory.hpp"

namespace sim {

struct Packet;
class IRoutingDevice;

/**
 * Unidirectional link from the source to a_next
 */
class ILink : public Identifiable {
public:
    virtual ~ILink() = default;

    /**
     * Update the source egress delay and schedule the arrival event
     * based on the egress queueing and transmission delays.
     */
    virtual void schedule_arrival(Packet packet) = 0;

    /**
     * Removes packet from the source egress queue.
     */
    virtual void process_arrival(Packet packet) = 0;

    virtual std::optional<Packet> get_packet() = 0;
    virtual std::shared_ptr<IRoutingDevice> get_from() const = 0;
    virtual std::shared_ptr<IRoutingDevice> get_to() const = 0;
};

}  // namespace sim
