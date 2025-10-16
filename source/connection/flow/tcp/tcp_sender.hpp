#pragma once
#include "event/event.hpp"
#include "i_tcp_cc.hpp"
#include "metrics/packet_reordering/simple_packet_reordering.hpp"
#include "tcp_common.hpp"

namespace sim {

class TcpSender;
using TcpSenderPtr = std::shared_ptr<TcpSender>;

class TcpSender : public std::enable_shared_from_this<TcpSender> {
public:
    static TcpSenderPtr create(TcpCommonPtr a_common,
                               std::unique_ptr<ITcpCC> a_cc,
                               SizeByte a_packet_size);

    SizeByte get_sending_quota() const;
    void send_data(SizeByte data);

    void update_rto_on_ack();
    void send_packet_now(Packet packet);
    void retransmit_packet(PacketNum packet_num);

private:
    TcpSender(TcpCommonPtr a_common, std::unique_ptr<ITcpCC> a_cc,
              SizeByte a_packet_size);

    class SendAtTime;
    class Timeout;

    // TODO: make it private
public:
    void set_avg_rtt_if_present(Packet& packet);
    Packet generate_data_packet(PacketNum packet_num);

    void update_rto_on_timeout();

    // TODO: make it private
public:
    TcpCommonPtr m_common;

    // Congestion control module
    std::unique_ptr<ITcpCC> m_cc;
    SizeByte m_packet_size;

    std::optional<TimeNs> m_first_send_time;
    std::optional<TimeNs> m_last_send_time;

    // is send_packet called at least once
    TimeNs m_last_ack_arrive_time;

    TimeNs m_current_rto;
    TimeNs m_max_rto;
    // is in STEADY state (after first ACK with valid RTT)
    bool m_rto_steady;

    std::uint32_t m_packets_in_flight;
    SizeByte m_delivered_data_size;
    PacketNum m_next_packet_num;

    // Contains numbers of all delivered acks
    std::set<PacketNum> m_acked;

    SimplePacketReordering m_packet_reordering;
    utils::Statistics<TimeNs> m_rtt_statistics;
};

}  // namespace sim