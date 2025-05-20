#pragma once

#include <cstdint>
#include <optional>
#include <queue>

#include "link.hpp"

namespace sim {

class RoutingModule;

/**
 * Unidirectional link from the source to a_next
 */
class ExpressPassLink : public ILink, public std::enable_shared_from_this<ExpressPassLink> {
public:
    ExpressPassLink(std::weak_ptr<IRoutingDevice> a_from,
         std::weak_ptr<IRoutingDevice> a_to, std::uint32_t a_speed_mbps = 1,
         Time a_delay = 0);
    ~ExpressPassLink() = default;

    /**
     * Update the source egress delay and schedule the arrival event
     * based on the egress queueing and transmission delays.
     */
    void schedule_arrival(Time current_time, Packet packet) final;

    /**
     * Removes packet from the source egress queue.
     */
    void process_arrival(Packet packet) final;

    std::optional<Packet> get_packet() final;

    std::shared_ptr<IRoutingDevice> get_from() const final;
    std::shared_ptr<IRoutingDevice> get_to() const final;

    Id get_id() const final;
    Size get_transmission() { return m_total_transmission; };

private:
    Time get_transmission_time(const Packet& packet) const;

    std::weak_ptr<IRoutingDevice> m_from;
    std::weak_ptr<IRoutingDevice> m_to;
    std::uint32_t m_speed_mbps;
    Time m_src_egress_delay;
    Time m_transmission_delay;
    Id m_id;

    std::uint32_t m_max_credit_queue_capacity = 8;
    std::uint32_t m_current_credit_queue_capacity = 0;
    Time m_next_credit_can_be_sent = 0;
    // Queue at the ingress port of the m_next device
    std::queue<Packet> m_next_ingress;

    Size m_total_transmission = 0;
    Size m_data_transmission = 0;
};

}  // namespace sim
