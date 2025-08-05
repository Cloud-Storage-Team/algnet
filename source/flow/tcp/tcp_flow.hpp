#pragma once

#include <type_traits>

#include "device/interfaces/i_host.hpp"
#include "flow/i_flow.hpp"
#include "i_tcp_cc.hpp"
#include "metrics/metrics_collector.hpp"
#include "metrics/packet_reordering/simple_packet_reordering.hpp"
#include "packet.hpp"
#include "scheduler.hpp"
#include "utils/flag_manager.hpp"
#include "utils/statistics.hpp"

namespace sim {

class TcpFlow : public IFlow, public std::enable_shared_from_this<TcpFlow> {
public:
    TcpFlow(Id a_id, std::shared_ptr<IHost> a_src,
            std::shared_ptr<IHost> a_dest, std::unique_ptr<ITcpCC> a_cc,
            SizeByte a_packet_size, std::uint32_t a_packets_to_send,
            bool a_ecn_capable = true);

    void start() final;
    void update(Packet packet) final;

    SizeByte get_delivered_data_size() const final;
    std::shared_ptr<IHost> get_sender() const final;
    std::shared_ptr<IHost> get_receiver() const;
    Id get_id() const final;
    SizeByte get_delivered_bytes() const;
    std::string to_string() const;

private:
    static std::string m_packet_type_label;
    enum PacketType { ACK, DATA, ENUM_SIZE };

    class SendAtTime;
    class Timeout;

    Packet create_packet(PacketNum packet_num);
    Packet generate_packet();
    TimeNs get_max_timeout() const;
    void send_packet_now(Packet packet);
    void send_packets();
    void retransmit_packet(PacketNum packet_num);
    static void initialize_flag_manager();

    static bool m_is_flag_manager_initialized;
    static FlagManager<std::string, PacketFlagsBase> m_flag_manager;

    Id m_id;

    std::weak_ptr<IHost> m_src;
    std::weak_ptr<IHost> m_dest;

    // Congestion control module
    std::unique_ptr<ITcpCC> m_cc;

    SizeByte m_packet_size;
    std::uint32_t m_packets_to_send;
    bool m_ecn_capable;

    std::uint32_t m_packets_in_flight;
    SizeByte m_delivered_data_size;
    PacketNum m_next_packet_num;

    // Contains numbers of all delivered acks
    std::set<PacketNum> m_acked;

    SimplePacketReordering m_packet_reordering;
    utils::Statistics<TimeNs> m_rtt_statistics;
};

}  // namespace sim
