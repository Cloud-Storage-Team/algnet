#pragma once

#include <type_traits>

#include "connection/flow/i_flow.hpp"
#include "connection/i_connection.hpp"
#include "device/interfaces/i_host.hpp"
#include "i_tcp_cc.hpp"
#include "metrics/packet_reordering/simple_packet_reordering.hpp"
#include "packet.hpp"
#include "utils/flag_manager.hpp"
#include "utils/packet_num_monitor.hpp"
#include "utils/str_expected.hpp"

namespace sim {

class TcpFlow : public IFlow, public std::enable_shared_from_this<TcpFlow> {
public:
    TcpFlow(Id a_id, std::shared_ptr<IConnection> a_conn,
            std::unique_ptr<ITcpCC> a_cc, SizeByte a_packet_size,
            bool a_ecn_capable = true);
    void update(Packet packet) final;
    void send_data(SizeByte data) final;

    SizeByte get_packet_size() const final;
    SizeByte get_total_data_size_added_from_conn() const final;
    SizeByte get_sent_data_size() const final;
    SizeByte get_delivered_data_size() const final;
    uint32_t retransmit_count() const final;

    SizeByte get_sending_quota() const final;

    std::optional<TimeNs> get_last_rtt() const final;
    // Returns time elapced from flow start (firsrt call of send_packet)
    // to last update call
    TimeNs get_fct() const final;

    const BaseFlagManager& get_flag_manager() const final;

    std::shared_ptr<IHost> get_sender() const final;
    std::shared_ptr<IHost> get_receiver() const final;

    Id get_id() const final;
    std::string to_string() const;

private:
    // common part
    static constexpr bool M_COLLECTIVE_ACK_SUPPORT = false;
    static void initialize_flag_manager();

    static std::string m_packet_type_label;
    enum PacketType { ACK, COLLECTIVE_ACK, DATA, ENUM_SIZE };

    static std::string m_ack_ttl_label;

    static bool m_is_flag_manager_initialized;
    static FlagManager<std::string, PacketFlagsBase> m_flag_manager;
    const static inline TTL M_MAX_TTL = 31;

    Id m_id;
    std::shared_ptr<IConnection> m_connection;

    std::weak_ptr<IHost> m_src;
    std::weak_ptr<IHost> m_dest;
    bool m_ecn_capable;

private:
    // sender part
    class SendAtTime;
    class Timeout;

    void process_single_ack(Packet ack);
    void process_collective_ack(Packet ack);
    void process_ack(Packet ack, std::size_t confirm_count);

    Packet generate_data_packet(PacketNum packet_num);
    void set_avg_rtt_if_present(Packet& packet);
    void update_rto_on_timeout();
    void update_rto_on_ack();
    void send_packet_now(Packet packet);
    void retransmit_packet(PacketNum packet_num);

    // Congestion control module
    std::unique_ptr<ITcpCC> m_cc;

    // is send_packet called at least once
    bool m_sending_started;
    TimeNs m_init_time;
    TimeNs m_last_ack_arrive_time;

    std::optional<TimeNs> m_last_send_time;

    SizeByte m_packet_size;

    TimeNs m_current_rto;
    TimeNs m_max_rto;
    // is in STEADY state (after first ACK with valid RTT)
    bool m_rto_steady;
    std::uint32_t m_retransmit_count;

    std::uint32_t m_packets_in_flight;
    SizeByte m_total_data_from_conn;
    SizeByte m_delivered_data_size;
    SizeByte m_sent_data_size;
    PacketNum m_next_packet_num;

    // Contains numbers of all delivered acks
    PacketNumMonitor m_ack_monitor;

    SimplePacketReordering m_packet_reordering;
    utils::Statistics<TimeNs> m_rtt_statistics;

private:
    // receiver part
    PacketNumMonitor m_data_packets_monitor;
    void process_data_packet(Packet data_packet);
    Packet create_ack(Packet data);
};

}  // namespace sim
