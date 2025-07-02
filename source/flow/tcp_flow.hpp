#pragma once

#include "device/interfaces/i_host.hpp"
#include "flow/interfaces/i_tcp_flow.hpp"
#include "packet.hpp"
#include "utils/flag_manager.hpp"

namespace sim {

class TcpFlow : public ITcpFlow, public std::enable_shared_from_this<TcpFlow> {
public:
    TcpFlow(Id a_id, std::shared_ptr<IHost> a_src,
            std::shared_ptr<IHost> a_dest, Size a_packet_size,
            Time a_delay_between_packets, std::uint32_t a_packets_to_send,
            Time a_delay_threshold = 4000, std::uint32_t a_ssthresh = 8,
            bool a_ecn_capable = true);

    void start() final;
    Time create_new_data_packet() final;

    void update(Packet packet, DeviceType type) final;
    std::shared_ptr<IHost> get_sender() const final;
    std::shared_ptr<IHost> get_receiver() const final;
    Id get_id() const final;
    double get_cwnd() const final;

    std::string to_string() const;

private:
    static std::string packet_type_label;
    enum PacketType { ACK, DATA, ENUM_SIZE };

    Packet generate_packet();

    bool try_to_put_data_to_device();

    static void initialize_flag_manager();
    static bool m_is_initialized;
    static FlagManager<std::string> m_flags;

    Id m_id;

    std::weak_ptr<IHost> m_src;
    std::weak_ptr<IHost> m_dest;

    bool m_ecn_capable;
    Size m_packet_size;
    Time m_delay_between_packets;
    std::uint32_t m_packets_to_send;
    Time m_delay_threshold;  // delay threshold for update

    double m_ssthresh;  // Slow start threshold
    double m_cwnd;      // Congestion window

    std::uint32_t m_packets_in_flight;
    std::uint32_t m_packets_acked;
    Size m_sent_bytes;
};
}  // namespace sim
