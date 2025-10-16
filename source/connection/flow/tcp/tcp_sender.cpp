#include "tcp_sender.hpp"

#include "metrics/metrics_collector.hpp"
#include "scheduler.hpp"
#include "utils/avg_rtt_packet_flag.hpp"

namespace sim {

TcpSenderPtr TcpSender::create(TcpCommonPtr a_common,
                               std::unique_ptr<ITcpCC> a_cc,
                               SizeByte a_packet_size) {
    return std::shared_ptr<TcpSender>(
        new TcpSender(a_common, std::move(a_cc), a_packet_size));
}

TcpSender::TcpSender(TcpCommonPtr a_common, std::unique_ptr<ITcpCC> a_cc,
                     SizeByte a_packet_size)
    : m_common(a_common),
      m_cc(std::move(a_cc)),
      m_packet_size(a_packet_size),
      m_first_send_time(std::nullopt),
      m_last_send_time(std::nullopt),
      m_last_ack_arrive_time(0),
      m_current_rto(2000),
      m_max_rto(Time<Second>(1)),
      m_rto_steady(false),
      m_packets_in_flight(0),
      m_delivered_data_size(0),
      m_next_packet_num(0) {}

void TcpSender::set_avg_rtt_if_present(Packet& packet) {
    std::optional<TimeNs> avg_rtt = m_rtt_statistics.get_mean();
    if (avg_rtt.has_value()) {
        set_avg_rtt_flag(m_common->flag_manager, packet.flags, avg_rtt.value());
    }
}

Packet TcpSender::generate_data_packet(PacketNum packet_num) {
    Packet packet;
    m_common->flag_manager.set_flag(packet.flags, TcpCommon::packet_type_label,
                                    TcpCommon::PacketType::DATA);
    set_avg_rtt_if_present(packet);
    packet.size = m_packet_size;
    packet.flow_id = m_common->id;
    {
        // set source id
        std::weak_ptr<IHost> sender = m_common->sender;
        if (!sender.expired()) {
            packet.source_id = sender.lock()->get_id();
        } else {
            LOG_ERROR(fmt::format(
                "Sender expired for flow {}; sender_id does not set to "
                "data packet {}",
                m_common->id, packet.to_string()));
        }
    }
    {
        // set dest id
        std::weak_ptr<IHost> receiver = m_common->receiver;
        if (!receiver.expired()) {
            packet.dest_id = receiver.lock()->get_id();
        } else {
            LOG_ERROR(fmt::format(
                "Receiver expider for flow {}; receiver_id does not set to "
                "data packet {}",
                m_common->id, packet.to_string()));
        }
    }
    packet.packet_num = packet_num;
    packet.delivered_data_size_at_origin = m_delivered_data_size;
    packet.generated_time = Scheduler::get_instance().get_current_time();
    packet.ttl = TcpCommon::MAX_TTL;
    packet.ecn_capable_transport = m_common->ecn_capable;
    return packet;
}

// Before the first ACK: exponential growth by timeout
void TcpSender::update_rto_on_timeout() {
    if (!m_rto_steady) {
        m_current_rto = std::min(m_current_rto * 2, m_max_rto);
    }
    // in STEADY, don't touch RTO by timeout
}

TcpSender::SendAtTime::SendAtTime(TimeNs a_time,
                                  std::weak_ptr<TcpSender> a_sender,
                                  Packet a_packet)
    : Event(a_time), m_sender(a_sender), m_packet(std::move(a_packet)) {}

void TcpSender::SendAtTime::operator()() {
    if (m_sender.expired()) {
        LOG_ERROR("Pointer to flow expired");
        return;
    }
    auto sender = m_sender.lock();
    sender->send_packet_now(std::move(m_packet));
}

class TcpSender::Timeout : public Event {
public:
    Timeout(TimeNs a_time, std::weak_ptr<TcpSender> a_flow,
            PacketNum a_packet_num)
        : Event(a_time), m_sender(a_flow), m_packet_num(a_packet_num) {}

    void operator()() {
        if (m_sender.expired()) {
            LOG_ERROR("Pointer to flow expired");
            return;
        }
        auto sender = m_sender.lock();

        if (sender->m_acked.contains(m_packet_num)) {
            return;
        }
        LOG_WARN(
            fmt::format("Timeout for packet number {} expired; looks "
                        "like packet loss",
                        m_packet_num));
        sender->update_rto_on_timeout();
        sender->m_cc->on_timeout();
        sender->retransmit_packet(m_packet_num);
    }

private:
    std::weak_ptr<TcpSender> m_sender;
    PacketNum m_packet_num;
};

// After ACK with a valid RTT: formula + transition to STEADY (once)
void TcpSender::update_rto_on_ack() {
    auto mean = m_rtt_statistics.get_mean().value();
    TimeNs std = m_rtt_statistics.get_std().value();
    m_current_rto = std::min(mean * 2 + std * 4, m_max_rto);
    m_rto_steady = true;
}

void TcpSender::send_packet_now(Packet packet) {
    TimeNs current_time = Scheduler::get_instance().get_current_time();

    if (m_last_send_time.has_value()) {
        MetricsCollector::get_instance().add_packet_spacing(
            m_common->id, current_time,
            current_time - m_last_send_time.value());
    }
    m_last_send_time = current_time;
    Scheduler::get_instance().add<Timeout>(current_time + m_current_rto,
                                           this->shared_from_this(),
                                           packet.packet_num);

    packet.sent_time = current_time;
    m_common->sender.lock()->enqueue_packet(std::move(packet));
}

void TcpSender::retransmit_packet(PacketNum packet_num) {
    Packet packet = generate_data_packet(packet_num);
    send_packet_now(std::move(packet));
}

SizeByte TcpSender::get_sending_quota() const {
    const double cwnd = m_cc->get_cwnd();

    // Effective window: the whole part of cwnd; if cwnd < 1 and inflight == 0,
    // allow 1 packet
    std::uint32_t effective_cwnd = static_cast<std::uint32_t>(std::floor(cwnd));
    if (m_packets_in_flight == 0 && cwnd < 1.0) {
        effective_cwnd = 1;
    }
    const std::uint32_t quota_pkts =
        (effective_cwnd > m_packets_in_flight)
            ? (effective_cwnd - m_packets_in_flight)
            : 0;

    // Quota in bytes, multiple of the packet size
    return quota_pkts * m_packet_size;
}

}  // namespace sim