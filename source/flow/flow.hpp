#pragma once

#include <queue>

#include "flow/interfaces/i_flow.hpp"

namespace sim {

class Flow : public IFlow, public std::enable_shared_from_this<Flow> {
public:
    Flow(Id a_id, std::shared_ptr<ISender> a_src,
         std::shared_ptr<IReceiver> a_dest, Size a_packet_size,
         Time a_delay_between_packets, std::uint32_t a_packets_to_send);
    virtual ~Flow() = default;

    // Start at time
    void start() final;

    Time create_new_data_packet() final;

    // Update the internal state according to some congestion control algorithm
    // Call try_to_generate upon the update
    void update(Packet packet, DeviceType type) final;
    std::uint32_t get_updates_number() const;

    std::shared_ptr<ISender> get_sender() const final;
    std::shared_ptr<IReceiver> get_receiver() const final;

    Id get_id() const final;

private:
    Time put_data_to_device();
    void schedule_packet_generation(Time time);
    Packet generate_packet();

    Id m_id;
    std::weak_ptr<ISender> m_src;
    std::weak_ptr<IReceiver> m_dest;
    Size m_packet_size;
    Time m_delay_between_packets;
    std::uint32_t m_updates_number;
    std::uint32_t m_packets_to_send;
    Size m_sent_bytes;

    std::queue<Packet> m_sending_buffer;
};

}  // namespace sim
