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
    : m_common(
          std::make_shared<TcpCommon>(std::move(a_id), a_conn, a_ecn_capable)),
      m_sender(m_common, a_conn->get_sender(), std::move(a_cc), a_packet_size),
      m_receiver(m_common, a_conn->get_receiver()) {}

SizeByte TcpFlow::get_delivered_data_size() const {
    return m_sender.m_delivered_data_size;
}

TimeNs TcpFlow::get_fct() const {
    if (!m_sender.m_first_send_time) {
        return TimeNs(0);
    }
    return m_sender.m_last_send_time.value() -
           m_sender.m_first_send_time.value();
}

const BaseFlagManager& TcpFlow::get_flag_manager() const {
    return m_common->flag_manager;
}

std::shared_ptr<IHost> TcpFlow::get_sender() const {
    return m_sender.m_sender.lock();
}

std::shared_ptr<IHost> TcpFlow::get_receiver() const {
    return m_receiver.m_receiver.lock();
}

Id TcpFlow::get_id() const { return m_common->id; }

SizeByte TcpFlow::get_delivered_bytes() const {
    return m_sender.m_delivered_data_size;
}

SizeByte TcpFlow::get_sending_quota() const {
    const double cwnd = m_sender.m_cc->get_cwnd();

    // Effective window: the whole part of cwnd; if cwnd < 1 and inflight == 0,
    // allow 1 packet
    std::uint32_t effective_cwnd = static_cast<std::uint32_t>(std::floor(cwnd));
    if (m_sender.m_packets_in_flight == 0 && cwnd < 1.0) {
        effective_cwnd = 1;
    }
    const std::uint32_t quota_pkts =
        (effective_cwnd > m_sender.m_packets_in_flight)
            ? (effective_cwnd - m_sender.m_packets_in_flight)
            : 0;

    // Quota in bytes, multiple of the packet size
    return quota_pkts * m_sender.m_packet_size;
}

void TcpFlow::set_avg_rtt_if_present(Packet& packet) {
    std::optional<TimeNs> avg_rtt = m_sender.m_rtt_statistics.get_mean();
    if (avg_rtt.has_value()) {
        set_avg_rtt_flag(m_common->flag_manager, packet.flags, avg_rtt.value());
    }
}

Packet TcpFlow::generate_data_packet(PacketNum packet_num) {
    Packet packet;
    m_common->flag_manager.set_flag(packet.flags, TcpCommon::packet_type_label,
                                    TcpCommon::PacketType::DATA);
    set_avg_rtt_if_present(packet);
    packet.size = m_sender.m_packet_size;
    packet.flow = this;
    packet.source_id = get_sender()->get_id();
    packet.dest_id = get_receiver()->get_id();
    packet.packet_num = packet_num;
    packet.delivered_data_size_at_origin = m_sender.m_delivered_data_size;
    packet.generated_time = Scheduler::get_instance().get_current_time();
    packet.ttl = TcpCommon::MAX_TTL;
    packet.ecn_capable_transport = m_common->ecn_capable;
    return packet;
}

void TcpFlow::send_data(SizeByte data) {
    TimeNs now = Scheduler::get_instance().get_current_time();

    if (!m_sender.m_first_send_time) {
        m_sender.m_first_send_time = now;
    }

    SizeByte quota = get_sending_quota();
    if (data > quota) {
        throw std::runtime_error(fmt::format(
            "Trying to send {} bytes on flow {} with quota {} bytes",
            data.value(), m_common->id, quota.value()));
    }

    while (data != SizeByte(0)) {
        Packet packet = generate_data_packet(m_sender.m_next_packet_num++);
        TimeNs pacing_delay = m_sender.m_cc->get_pacing_delay();
        if (pacing_delay == TimeNs(0)) {
            send_packet_now(std::move(packet));
        } else {
            Scheduler::get_instance().add<SendAtTime>(now + pacing_delay,
                                                      this->shared_from_this(),
                                                      std::move(packet));
        }
        data -= std::min(data, m_sender.m_packet_size);
        m_sender.m_packets_in_flight++;
    }
}

Packet TcpFlow::create_ack(Packet data) {
    Packet ack;
    ack.packet_num = data.packet_num;
    ack.source_id = m_receiver.m_receiver.lock()->get_id();
    ack.dest_id = m_sender.m_sender.lock()->get_id();
    ack.size = SizeByte(1);
    ack.flow = this;
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
    set_avg_rtt_if_present(ack);
    return ack;
}

Packet TcpFlow::generate_packet() {
    return generate_data_packet(m_sender.m_next_packet_num++);
}

class TcpFlow::SendAtTime : public Event {
public:
    SendAtTime(TimeNs a_time, std::weak_ptr<TcpFlow> a_flow, Packet a_packet)
        : Event(a_time), m_flow(a_flow), m_packet(std::move(a_packet)) {}

    void operator()() final {
        if (m_flow.expired()) {
            LOG_ERROR("Pointer to flow expired");
            return;
        }
        auto flow = m_flow.lock();
        flow->send_packet_now(std::move(m_packet));
    }

private:
    std::weak_ptr<TcpFlow> m_flow;
    Packet m_packet;
};

class TcpFlow::Timeout : public Event {
public:
    Timeout(TimeNs a_time, std::weak_ptr<TcpFlow> a_flow,
            PacketNum a_packet_num)
        : Event(a_time), m_flow(a_flow), m_packet_num(a_packet_num) {}

    void operator()() {
        if (m_flow.expired()) {
            LOG_ERROR("Pointer to flow expired");
            return;
        }
        auto flow = m_flow.lock();

        if (flow->m_sender.m_acked.contains(m_packet_num)) {
            return;
        }
        LOG_WARN(
            fmt::format("Timeout for packet number {} expired; looks "
                        "like packet loss",
                        m_packet_num));
        flow->update_rto_on_timeout();
        flow->m_sender.m_cc->on_timeout();
        flow->retransmit_packet(m_packet_num);
    }

private:
    std::weak_ptr<TcpFlow> m_flow;
    PacketNum m_packet_num;
};

void TcpFlow::update(Packet packet) {
    TcpCommon::PacketType type =
        static_cast<TcpCommon::PacketType>(m_common->flag_manager.get_flag(
            packet.flags, TcpCommon::packet_type_label));
    TimeNs current_time = Scheduler::get_instance().get_current_time();
    if (packet.dest_id == m_sender.m_sender.lock()->get_id() &&
        type == TcpCommon::PacketType::ACK) {
        if (current_time < packet.sent_time) {
            LOG_ERROR("Packet " + packet.to_string() +
                      " current time less that sending time; ignored");
            return;
        }

        m_sender.m_last_ack_arrive_time = current_time;

        if (m_sender.m_acked.contains(packet.packet_num)) {
            LOG_WARN(fmt::format("Got duplicate ack number {}; ignored",
                                 packet.packet_num));
            return;
        }

        TimeNs rtt = current_time - packet.sent_time;
        m_sender.m_rtt_statistics.add_record(rtt);
        update_rto_on_ack();  // update and transition to STEADY

        MetricsCollector::get_instance().add_RTT(packet.flow->get_id(),
                                                 current_time, rtt);
        m_sender.m_acked.insert(packet.packet_num);

        if (m_sender.m_packets_in_flight > 0) {
            m_sender.m_packets_in_flight--;
        }

        m_sender.m_cc->on_ack(rtt, m_sender.m_rtt_statistics.get_mean().value(),
                              packet.congestion_experienced);

        m_sender.m_delivered_data_size += m_sender.m_packet_size;

        SpeedGbps delivery_rate = (m_sender.m_delivered_data_size -
                                   packet.delivered_data_size_at_origin) /
                                  (current_time - packet.generated_time);
        MetricsCollector::get_instance().add_delivery_rate(
            packet.flow->get_id(), current_time, delivery_rate);

        MetricsCollector::get_instance().add_cwnd(m_common->id, current_time,
                                                  m_sender.m_cc->get_cwnd());
        if (m_common->connection.expired()) {
            LOG_ERROR(fmt::format(
                "Can not update coinnection from flow '{}': connection expired",
                m_common->id));
        } else {
            m_common->connection.lock()->update(shared_from_this());
        }
    } else if (packet.dest_id == m_receiver.m_receiver.lock()->get_id() &&
               type == TcpCommon::PacketType::DATA) {
        m_sender.m_packet_reordering.add_record(packet.packet_num);
        MetricsCollector::get_instance().add_packet_reordering(
            m_common->id, current_time, m_sender.m_packet_reordering.value());

        Packet ack = create_ack(std::move(packet));

        m_receiver.m_receiver.lock()->enqueue_packet(ack);
    }
}

std::optional<TimeNs> TcpFlow::get_last_rtt() const {
    return m_sender.m_rtt_statistics.get_last();
}

std::string TcpFlow::to_string() const {
    std::ostringstream oss;
    oss << "[TcpFlow; ";
    oss << "Id:" << m_common->id;
    oss << ", src id: " << m_sender.m_sender.lock()->get_id();
    oss << ", dest id: " << m_receiver.m_receiver.lock()->get_id();
    oss << ", CC module: " << m_sender.m_cc->to_string();
    oss << ", packet size: " << m_sender.m_packet_size;
    oss << ", packets in flight: " << m_sender.m_packets_in_flight;
    oss << ", acked packets: " << m_sender.m_delivered_data_size;
    oss << "]";
    return oss.str();
}

// Before the first ACK: exponential growth by timeout
void TcpFlow::update_rto_on_timeout() {
    if (!m_sender.m_rto_steady) {
        m_sender.m_current_rto =
            std::min(m_sender.m_current_rto * 2, m_sender.m_max_rto);
    }
    // in STEADY, don't touch RTO by timeout
}

// After ACK with a valid RTT: formula + transition to STEADY (once)
void TcpFlow::update_rto_on_ack() {
    auto mean = m_sender.m_rtt_statistics.get_mean().value();
    TimeNs std = m_sender.m_rtt_statistics.get_std().value();
    m_sender.m_current_rto = std::min(mean * 2 + std * 4, m_sender.m_max_rto);
    m_sender.m_rto_steady = true;
}

void TcpFlow::send_packet_now(Packet packet) {
    TimeNs current_time = Scheduler::get_instance().get_current_time();

    if (m_sender.m_last_send_time.has_value()) {
        MetricsCollector::get_instance().add_packet_spacing(
            m_common->id, current_time,
            current_time - m_sender.m_last_send_time.value());
    }
    m_sender.m_last_send_time = current_time;
    Scheduler::get_instance().add<Timeout>(
        current_time + m_sender.m_current_rto, this->shared_from_this(),
        packet.packet_num);

    packet.sent_time = current_time;
    m_sender.m_sender.lock()->enqueue_packet(std::move(packet));
}

void TcpFlow::retransmit_packet(PacketNum packet_num) {
    Packet packet = generate_data_packet(packet_num);
    send_packet_now(std::move(packet));
}

}  // namespace sim
