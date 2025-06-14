#pragma once

#include "flow/interfaces/i_tcp_flow.hpp"

namespace sim {

class TcpFlow : public ITcpFlow, public std::enable_shared_from_this<TcpFlow> {
public:
    // TODO: delete a_delay_threshold default value
    TcpFlow(Id a_id, std::shared_ptr<ISender> a_src,
            std::shared_ptr<IReceiver> a_dest, Size a_packet_size,
            Time a_delay_between_packets, std::uint32_t a_packets_to_send,
            Time a_delay_threshold = 4000, std::uint32_t a_ssthresh = 8);

    void start() final;
    Time create_new_data_packet() final;

    void update(Packet packet, DeviceType type) final;
    std::shared_ptr<ISender> get_sender() const final;
    std::shared_ptr<IReceiver> get_receiver() const final;
    Id get_id() const final;
    double get_cwnd() const final;

    std::string to_string() const;

private:
    Packet generate_packet();

    bool try_to_put_data_to_device();

    Id m_id;
    std::weak_ptr<ISender> m_src;
    std::weak_ptr<IReceiver> m_dest;
    Size m_packet_size;

    Time m_delay_between_packets;
    std::uint32_t m_packets_to_send;
    std::uint32_t m_delay_threshold;  // delay threshold for update
    double m_ssthresh;                // Slow start threshold

    double m_cwnd;  // Congestion window
    std::uint32_t m_packets_in_flight;
    std::uint32_t m_packets_acked;

    Size m_sent_bytes;
};
}  // namespace sim
