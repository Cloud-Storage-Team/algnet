#include "connection/flow/tcp/tcp_flow.hpp"

#include "connection/i_connection.hpp"
#include "metrics/metrics_collector.hpp"
#include "packet.hpp"
#include "scheduler.hpp"
#include "utils/avg_rtt_packet_flag.hpp"

namespace sim {

TcpFlow::TcpFlow(Id a_id, std::shared_ptr<IConnection> a_conn,
                 std::unique_ptr<ITcpCC> a_cc, SizeByte a_packet_size,
                 bool a_ecn_capable)
    : m_common(std::make_shared<TcpCommon>(
          std::move(a_id), a_conn, a_conn->get_sender(), a_conn->get_receiver(),
          a_ecn_capable)),
      m_sender(TcpSender::create(m_common, std::move(a_cc), a_packet_size)),
      m_receiver(m_common) {}

SizeByte TcpFlow::get_delivered_data_size() const {
    return m_sender->m_delivered_data_size;
}

TimeNs TcpFlow::get_fct() const {
    if (!m_sender->m_first_send_time) {
        return TimeNs(0);
    }
    return m_sender->m_last_send_time.value() -
           m_sender->m_first_send_time.value();
}

const BaseFlagManager& TcpFlow::get_flag_manager() const {
    return m_common->flag_manager;
}

std::shared_ptr<IHost> TcpFlow::get_sender() const {
    return m_common->sender.lock();
}

std::shared_ptr<IHost> TcpFlow::get_receiver() const {
    return m_common->receiver.lock();
}

Id TcpFlow::get_id() const { return m_common->id; }

SizeByte TcpFlow::get_delivered_bytes() const {
    return m_sender->m_delivered_data_size;
}

SizeByte TcpFlow::get_sending_quota() const {
    const double cwnd = m_sender->m_cc->get_cwnd();

    // Effective window: the whole part of cwnd; if cwnd < 1 and inflight == 0,
    // allow 1 packet
    std::uint32_t effective_cwnd = static_cast<std::uint32_t>(std::floor(cwnd));
    if (m_sender->m_packets_in_flight == 0 && cwnd < 1.0) {
        effective_cwnd = 1;
    }
    const std::uint32_t quota_pkts =
        (effective_cwnd > m_sender->m_packets_in_flight)
            ? (effective_cwnd - m_sender->m_packets_in_flight)
            : 0;

    // Quota in bytes, multiple of the packet size
    return quota_pkts * m_sender->m_packet_size;
}

void TcpFlow::send_data(SizeByte data) {
    TimeNs now = Scheduler::get_instance().get_current_time();

    if (!m_sender->m_first_send_time) {
        m_sender->m_first_send_time = now;
    }

    SizeByte quota = get_sending_quota();
    if (data > quota) {
        throw std::runtime_error(fmt::format(
            "Trying to send {} bytes on flow {} with quota {} bytes",
            data.value(), m_common->id, quota.value()));
    }

    while (data != SizeByte(0)) {
        Packet packet =
            m_sender->generate_data_packet(m_sender->m_next_packet_num++);
        TimeNs pacing_delay = m_sender->m_cc->get_pacing_delay();
        if (pacing_delay == TimeNs(0)) {
            m_sender->send_packet_now(std::move(packet));
        } else {
            Scheduler::get_instance().add<TcpSender::SendAtTime>(
                now + pacing_delay, m_sender, std::move(packet));
        }
        data -= std::min(data, m_sender->m_packet_size);
        m_sender->m_packets_in_flight++;
    }
}

Packet TcpFlow::create_ack(Packet data) {
    Packet ack;
    ack.packet_num = data.packet_num;
    ack.source_id = m_common->receiver.lock()->get_id();
    ack.dest_id = m_common->sender.lock()->get_id();
    ack.size = SizeByte(1);
    ack.flow_id = m_common->id;
    ack.generated_time = data.generated_time;
    ack.sent_time = data.sent_time;
    ack.delivered_data_size_at_origin = data.delivered_data_size_at_origin;
    ack.ttl = TcpCommon::MAX_TTL;
    ack.ecn_capable_transport = data.ecn_capable_transport;
    ack.congestion_experienced = data.congestion_experienced;

    m_common->flag_manager.set_flag(ack.flags, TcpCommon::packet_type_label,
                                    TcpCommon::PacketType::ACK);
    m_common->flag_manager.set_flag(ack.flags, TcpCommon::ack_ttl_label,
                                    data.ttl);

    try {
        TimeNs rtt = get_avg_rtt_label(m_common->flag_manager, data.flags);
        set_avg_rtt_flag(m_common->flag_manager, ack.flags, rtt);
    } catch (const FlagNotSetException& e) {
        LOG_INFO(
            fmt::format("avg rtt flag does not set in data packet {} so it "
                        "will not be set in "
                        "ack {}",
                        data.to_string(), ack.to_string()));
    }
    return ack;
}

void TcpFlow::update(Packet packet) {
    TcpCommon::PacketType type =
        static_cast<TcpCommon::PacketType>(m_common->flag_manager.get_flag(
            packet.flags, TcpCommon::packet_type_label));
    TimeNs current_time = Scheduler::get_instance().get_current_time();
    if (packet.dest_id == m_common->sender.lock()->get_id() &&
        type == TcpCommon::PacketType::ACK) {
        if (current_time < packet.sent_time) {
            LOG_ERROR("Packet " + packet.to_string() +
                      " current time less that sending time; ignored");
            return;
        }

        m_sender->m_last_ack_arrive_time = current_time;

        if (m_sender->m_acked.contains(packet.packet_num)) {
            LOG_WARN(fmt::format("Got duplicate ack number {}; ignored",
                                 packet.packet_num));
            return;
        }

        TimeNs rtt = current_time - packet.sent_time;
        m_sender->m_rtt_statistics.add_record(rtt);
        m_sender->update_rto_on_ack();  // update and transition to STEADY

        MetricsCollector::get_instance().add_RTT(m_common->id, current_time,
                                                 rtt);
        m_sender->m_acked.insert(packet.packet_num);

        if (m_sender->m_packets_in_flight > 0) {
            m_sender->m_packets_in_flight--;
        }

        m_sender->m_cc->on_ack(rtt,
                               m_sender->m_rtt_statistics.get_mean().value(),
                               packet.congestion_experienced);

        m_sender->m_delivered_data_size += m_sender->m_packet_size;

        SpeedGbps delivery_rate = (m_sender->m_delivered_data_size -
                                   packet.delivered_data_size_at_origin) /
                                  (current_time - packet.generated_time);
        MetricsCollector::get_instance().add_delivery_rate(
            m_common->id, current_time, delivery_rate);

        MetricsCollector::get_instance().add_cwnd(m_common->id, current_time,
                                                  m_sender->m_cc->get_cwnd());
        if (m_common->connection.expired()) {
            LOG_ERROR(fmt::format(
                "Can not update coinnection from flow '{}': connection expired",
                m_common->id));
        } else {
            m_common->connection.lock()->update(shared_from_this());
        }
    } else if (packet.dest_id == m_common->receiver.lock()->get_id() &&
               type == TcpCommon::PacketType::DATA) {
        m_sender->m_packet_reordering.add_record(packet.packet_num);
        MetricsCollector::get_instance().add_packet_reordering(
            m_common->id, current_time, m_sender->m_packet_reordering.value());

        Packet ack = create_ack(std::move(packet));

        m_common->receiver.lock()->enqueue_packet(ack);
    }
}

std::optional<TimeNs> TcpFlow::get_last_rtt() const {
    return m_sender->m_rtt_statistics.get_last();
}

std::string TcpFlow::to_string() const {
    std::ostringstream oss;
    oss << "[TcpFlow; ";
    oss << "Id:" << m_common->id;
    oss << ", src id: " << m_common->sender.lock()->get_id();
    oss << ", dest id: " << m_common->receiver.lock()->get_id();
    oss << ", CC module: " << m_sender->m_cc->to_string();
    oss << ", packet size: " << m_sender->m_packet_size;
    oss << ", packets in flight: " << m_sender->m_packets_in_flight;
    oss << ", acked packets: " << m_sender->m_delivered_data_size;
    oss << "]";
    return oss.str();
}

}  // namespace sim
