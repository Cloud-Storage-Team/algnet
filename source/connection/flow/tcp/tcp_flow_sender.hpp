#pragma once
#include <optional>

#include "cc/i_tcp_cc.hpp"
#include "event/event.hpp"
#include "tcp_flow_common.hpp"
#include "utils/packet_num_monitor.hpp"

namespace sim {

class TcpFlowSender;
using TcpSenderPtr = std::shared_ptr<TcpFlowSender>;

class TcpFlowSender : public std::enable_shared_from_this<TcpFlowSender> {
public:
    static TcpSenderPtr create(TcpCommonPtr a_common,
                               std::unique_ptr<ITcpCC> a_cc,
                               SizeByte a_packet_size);

private:
    // because of inheritance from std::enable_shared_from_this
    TcpFlowSender(TcpCommonPtr a_common, std::unique_ptr<ITcpCC> a_cc,
                  SizeByte a_packet_size);

public:
    void process_ack_packet(Packet ack);
    void send_data(SizeByte data);

    SizeByte get_packet_size() const;
    SizeByte get_sent_data_size() const;
    SizeByte get_delivered_data_size() const;
    uint32_t get_retransmit_count() const;

    SizeByte get_sending_quota() const;
    std::optional<TimeNs> get_fct() const;
    std::optional<TimeNs> get_last_rtt() const;
    std::string to_string() const;

private:
    class SendAtTime;
    class Timeout;

    void update_rto_on_ack();
    void update_rto_on_timeout();

    Packet generate_data_packet(PacketNum packet_num);
    void send_packet_now(Packet packet);
    void retransmit_packet(PacketNum packet_num);

    void set_avg_rtt_if_present(Packet& packet);

private:
    TcpCommonPtr m_common;

    // Congestion control module
    std::unique_ptr<ITcpCC> m_cc;
    SizeByte m_packet_size;

    std::optional<TimeNs> m_first_send_time;
    std::optional<TimeNs> m_last_send_time;
    std::optional<TimeNs> m_last_ack_arrive_time;

    TimeNs m_current_rto;
    TimeNs m_max_rto;
    // is in STEADY state (after first ACK with valid RTT)
    bool m_rto_steady;
    std::uint32_t m_retransmit_count;

    std::uint32_t m_packets_in_flight;
    SizeByte m_sent_data_size;
    SizeByte m_delivered_data_size;
    PacketNum m_next_packet_num;

    // Contains numbers of all confirmed packets
    PacketNumMonitor m_ack_monitor;

    utils::Statistics<TimeNs> m_rtt_statistics;
};

}  // namespace sim