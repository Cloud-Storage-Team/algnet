#pragma once


#include <queue>

#include "device/interfaces/i_host.hpp"
#include "flow/interfaces/i_flow.hpp"

namespace sim {

class BasicFlow : public IFlow, public std::enable_shared_from_this<BasicFlow> {
public:
    BasicFlow(Id a_id, std::shared_ptr<IHost> a_src,
              std::shared_ptr<IHost> a_dest, Size a_packet_size,
              Time a_delay_between_packets, std::uint32_t a_packets_to_send);
    virtual ~BasicFlow() = default;

    // Start at time
    void start() final;

    Time create_new_data_packet() final;

    // Update the internal state according to some congestion control algorithm
    // Call try_to_generate upon the update
    void update(Packet packet, DeviceType type) final;
    std::uint32_t get_updates_number() const;

    std::shared_ptr<IHost> get_sender() const final;
    std::shared_ptr<IHost> get_receiver() const final;

    Id get_id() const final;

private:
    Time put_data_to_device();
    void schedule_packet_generation(Time time);
    Packet generate_packet();

    Id m_id;
    std::weak_ptr<IHost> m_src;
    std::weak_ptr<IHost> m_dest;
    Size m_packet_size;
    Time m_delay_between_packets;
    std::uint32_t m_updates_number;
    std::uint32_t m_packets_to_send;
    Size m_sent_bytes;

    std::queue<Packet> m_sending_buffer;
};

}  // namespace sim
