#pragma once
#include <cstdint>

#include "flow.hpp"

namespace sim {

class ExpressPassFlow : public IFlow, public std::enable_shared_from_this<Flow> {
public:
    Flow(std::shared_ptr<ISender> a_src, std::shared_ptr<IReceiver> a_dest,
         Size a_packet_size, Time a_delay_between_packets,
         std::uint32_t a_packets_to_send);
    virtual ~Flow() = default;

    // Start at time
    void start(Time time) final;

    // Try to generate a new packet if the internal state allows to do so.
    // by placing it into the flow buffer of the source node.
    // Schedule the next generation event.
    Time try_to_generate() final;

    // Update the internal state according to some congestion control algorithm
    // Call try_to_generate upon the update
    void update() final;
    std::uint32_t get_updates_number() const;

    std::shared_ptr<ISender> get_sender() const final;
    std::shared_ptr<IReceiver> get_receiver() const final;

private:
    void schedule_packet_generation(Time time);
    void generate_packet();

    std::shared_ptr<ISender> m_src;
    std::shared_ptr<IReceiver> m_dest;
    Size m_packet_size;
    Time m_delay_between_packets;
    std::uint32_t m_updates_number;
    std::uint32_t m_packets_to_send;
};

}  // namespace sim
