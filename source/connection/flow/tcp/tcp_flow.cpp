#include "connection/flow/tcp/tcp_flow.hpp"

#include "connection/i_connection.hpp"
#include "metrics/metrics_collector.hpp"
#include "packet.hpp"
#include "scheduler.hpp"
#include "utils/avg_rtt_packet_flag.hpp"

namespace sim {

std::string TcpFlow::m_packet_type_label = "type";
std::string TcpFlow::m_ack_ttl_label = "ack_ttl";
FlagManager<std::string, PacketFlagsBase> TcpFlow::m_flag_manager;
bool TcpFlow::m_is_flag_manager_initialized = false;

TcpFlow::TcpFlow(Id a_id, std::shared_ptr<IConnection> a_conn,
                 std::unique_ptr<ITcpCC> a_cc, SizeByte a_packet_size,
                 bool a_ecn_capable)
    : m_id(std::move(a_id)),
      m_connection(std::move(a_conn)),
      m_src(m_connection->get_sender()),
      m_dest(m_connection->get_receiver()),
      m_ecn_capable(a_ecn_capable),
      m_cc(std::move(a_cc)),
      m_sending_started(false),
      m_init_time(0),
      m_last_ack_arrive_time(0),
      m_last_send_time(std::nullopt),
      m_packet_size(a_packet_size),
      m_current_rto(TimeNs(2000)),
      m_max_rto(Time<Second>(1)),
      m_rto_steady(false),
      m_retransmit_count(0),
      m_packets_in_flight(0),
      m_delivered_data_size(0),
      m_sent_data_size(0),
      m_next_packet_num(0) {
    if (m_src.lock() == nullptr) {
        throw std::invalid_argument("Sender for TcpFlow is nullptr");
    }
    if (m_dest.lock() == nullptr) {
        throw std::invalid_argument("Receiver for TcpFlow is nullptr");
    }
    initialize_flag_manager();
}

void TcpFlow::update(Packet packet) {
    PacketType type = static_cast<PacketType>(
        m_flag_manager.get_flag(packet.flags, m_packet_type_label));
    if (m_src.expired()) {
        LOG_ERROR(fmt::format("Sender exprired for flow {}; ignore packet {}",
                              to_string(), packet.to_string()));
        return;
    }
    if (m_dest.expired()) {
        LOG_ERROR(fmt::format("Receiver exprired for flow {}; ignore packet {}",
                              to_string(), packet.to_string()));
        return;
    }
    if (packet.dest_id == m_src.lock()->get_id()) {
        switch (type) {
            case PacketType::ACK: {
                process_single_ack(std::move(packet));
                break;
            }
            case PacketType::COLLECTIVE_ACK: {
                process_collective_ack(std::move(packet));
                break;
            }
            default: {
                LOG_ERROR(fmt::format(
                    "Unexpected type of packet {} arrived to sender",
                    packet.to_string()));
                break;
            }
        }
    } else if (packet.dest_id == m_dest.lock()->get_id()) {
        switch (type) {
            case PacketType::DATA: {
                process_data_packet(std::move(packet));
                break;
            }
            default: {
                LOG_ERROR(fmt::format(
                    "Unexpected type of packet {} arrived to receiver",
                    packet.to_string()));
                break;
            }
        }
    } else {
        LOG_ERROR(
            fmt::format("Packet {} arrived to unexoected device (see dest_id)",
                        packet.to_string()));
    }
}

void TcpFlow::send_data(SizeByte data) {
    TimeNs now = Scheduler::get_instance().get_current_time();

    if (!m_sending_started) {
        m_init_time = now;
        m_sending_started = true;
    }

    SizeByte quota = get_sending_quota();
    if (data > quota) {
        throw std::runtime_error(fmt::format(
            "Trying to send {} bytes on flow {} with quota {} bytes",
            data.value(), m_id, quota.value()));
    }

    while (data != SizeByte(0)) {
        Packet packet = generate_data_packet(m_next_packet_num++);
        TimeNs pacing_delay = m_cc->get_pacing_delay();
        if (pacing_delay == TimeNs(0)) {
            send_packet_now(std::move(packet));
        } else {
            Scheduler::get_instance().add<SendAtTime>(now + pacing_delay,
                                                      this->shared_from_this(),
                                                      std::move(packet));
        }
        data -= std::min(data, m_packet_size);
        m_packets_in_flight++;
    }
}

SizeByte TcpFlow::get_packet_size() const { return m_packet_size; }

SizeByte TcpFlow::get_sent_data_size() const { return m_sent_data_size; }

SizeByte TcpFlow::get_delivered_data_size() const {
    return m_delivered_data_size;
}

uint32_t TcpFlow::retransmit_count() const { return m_retransmit_count; }

SizeByte TcpFlow::get_sending_quota() const {
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

std::optional<TimeNs> TcpFlow::get_last_rtt() const {
    return m_rtt_statistics.get_last();
}

TimeNs TcpFlow::get_fct() const {
    if (!m_sending_started) {
        return TimeNs(0);
    }
    return m_last_ack_arrive_time - m_init_time;
}

const BaseFlagManager& TcpFlow::get_flag_manager() const {
    return m_flag_manager;
}

std::shared_ptr<IHost> TcpFlow::get_sender() const { return m_src.lock(); }

std::shared_ptr<IHost> TcpFlow::get_receiver() const { return m_dest.lock(); }

Id TcpFlow::get_id() const { return m_id; }

std::string TcpFlow::to_string() const {
    std::ostringstream oss;
    oss << "[TcpFlow; ";
    oss << "Id:" << m_id;
    oss << ", src id: " << m_src.lock()->get_id();
    oss << ", dest id: " << m_dest.lock()->get_id();
    oss << ", CC module: " << m_cc->to_string();
    oss << ", packet size: " << m_packet_size;
    oss << ", packets in flight: " << m_packets_in_flight;
    oss << ", acked packets: " << m_delivered_data_size;
    oss << "]";
    return oss.str();
}

void TcpFlow::initialize_flag_manager() {
    if (!m_is_flag_manager_initialized) {
        if (!m_flag_manager.register_flag_by_amount(m_packet_type_label,
                                                    PacketType::ENUM_SIZE)) {
            throw std::runtime_error("Can not registrate packet type label");
        }
        if (!m_flag_manager.register_flag_by_amount(m_ack_ttl_label,
                                                    M_MAX_TTL + 1)) {
            throw std::runtime_error("Can not registrate ack ttl label");
        }
        if (!register_packet_avg_rtt_flag(m_flag_manager)) {
            throw std::runtime_error("Can not registrate packet avg rtt label");
        }
        m_is_flag_manager_initialized = true;
    }
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

        if (flow->m_acked.is_confirmed(m_packet_num)) {
            return;
        }
        LOG_WARN(
            fmt::format("Timeout for packet number {} expired; looks "
                        "like packet loss",
                        m_packet_num));
        flow->update_rto_on_timeout();
        flow->m_cc->on_timeout();
        flow->retransmit_packet(m_packet_num);
    }

private:
    std::weak_ptr<TcpFlow> m_flow;
    PacketNum m_packet_num;
};

void TcpFlow::process_single_ack(Packet ack) {
    bool confirmed = m_acked.confirm_one(ack.packet_num);
    process_ack(std::move(ack), confirmed);
}
void TcpFlow::process_collective_ack(Packet ack) {
    std::size_t confirmed = m_acked.confirm_to(ack.packet_num);
    process_ack(std::move(ack), confirmed);
}

void TcpFlow::process_ack(Packet ack, std::size_t confirm_count) {
    TimeNs current_time = Scheduler::get_instance().get_current_time();
    if (current_time < ack.sent_time) {
        LOG_ERROR("Packet " + ack.to_string() +
                  " current time less that sending time; ignored");
        return;
    }

    m_last_ack_arrive_time = current_time;

    if (confirm_count == 0) {
        LOG_WARN(fmt::format("Got ack number {} that confirm nothing; ignored",
                             ack.packet_num));
        return;
    }

    TimeNs rtt = current_time - ack.sent_time;
    m_rtt_statistics.add_record(rtt);
    update_rto_on_ack();  // update and transition to STEADY

    MetricsCollector::get_instance().add_RTT(ack.flow->get_id(), current_time,
                                             rtt);

    if (m_packets_in_flight > confirm_count) {
        m_packets_in_flight -= confirm_count;
    } else {
        m_packets_in_flight = 0;
    }

    m_cc->on_ack(rtt, m_rtt_statistics.get_mean().value(),
                 ack.congestion_experienced);

    m_delivered_data_size += m_packet_size * confirm_count;

    SpeedGbps delivery_rate =
        (m_delivered_data_size - ack.delivered_data_size_at_origin) /
        (current_time - ack.generated_time);
    MetricsCollector::get_instance().add_delivery_rate(
        ack.flow->get_id(), current_time, delivery_rate);

    MetricsCollector::get_instance().add_cwnd(m_id, current_time,
                                              m_cc->get_cwnd());
    m_connection->update(shared_from_this());
}

Packet TcpFlow::generate_data_packet(PacketNum packet_num) {
    Packet packet;
    m_flag_manager.set_flag(packet.flags, m_packet_type_label,
                            PacketType::DATA);
    set_avg_rtt_if_present(packet);
    packet.size = m_packet_size;
    packet.flow = this;
    packet.source_id = get_sender()->get_id();
    packet.dest_id = get_receiver()->get_id();
    packet.packet_num = packet_num;
    packet.delivered_data_size_at_origin = m_delivered_data_size;
    packet.generated_time = Scheduler::get_instance().get_current_time();
    packet.ttl = M_MAX_TTL;
    packet.ecn_capable_transport = m_ecn_capable;
    return packet;
}

void TcpFlow::set_avg_rtt_if_present(Packet& packet) {
    std::optional<TimeNs> avg_rtt = m_rtt_statistics.get_mean();
    if (avg_rtt.has_value()) {
        set_avg_rtt_flag(m_flag_manager, packet.flags, avg_rtt.value());
    }
}

// Before the first ACK: exponential growth by timeout
void TcpFlow::update_rto_on_timeout() {
    if (!m_rto_steady) {
        m_current_rto = std::min(m_current_rto * 2, m_max_rto);
    }
    // in STEADY, don't touch RTO by timeout
}

// After ACK with a valid RTT: formula + transition to STEADY (once)
void TcpFlow::update_rto_on_ack() {
    auto mean = m_rtt_statistics.get_mean().value();
    TimeNs std = m_rtt_statistics.get_std().value();
    m_current_rto = std::min(mean * 2 + std * 4, m_max_rto);
    m_rto_steady = true;
}

void TcpFlow::send_packet_now(Packet packet) {
    TimeNs current_time = Scheduler::get_instance().get_current_time();

    if (m_last_send_time.has_value()) {
        MetricsCollector::get_instance().add_packet_spacing(
            m_id, current_time, current_time - m_last_send_time.value());
    }
    m_last_send_time = current_time;
    Scheduler::get_instance().add<Timeout>(current_time + m_current_rto,
                                           this->shared_from_this(),
                                           packet.packet_num);
    m_sent_data_size += packet.size;

    packet.sent_time = current_time;
    m_src.lock()->enqueue_packet(std::move(packet));
}

void TcpFlow::retransmit_packet(PacketNum packet_num) {
    Packet packet = generate_data_packet(packet_num);
    send_packet_now(std::move(packet));
    m_retransmit_count++;
}

void TcpFlow::process_data_packet(Packet packet) {
    TimeNs current_time = Scheduler::get_instance().get_current_time();
    m_packet_reordering.add_record(packet.packet_num);
    MetricsCollector::get_instance().add_packet_reordering(
        m_id, current_time, m_packet_reordering.value());

    Packet ack = create_ack(std::move(packet));

    m_dest.lock()->enqueue_packet(ack);
}

Packet TcpFlow::create_ack(Packet data) {
    Packet ack;
    ack.packet_num = data.packet_num;
    ack.source_id = m_dest.lock()->get_id();
    ack.dest_id = m_src.lock()->get_id();
    ack.size = SizeByte(1);
    ack.flow = this;
    ack.generated_time = data.generated_time;
    ack.sent_time = data.sent_time;
    ack.delivered_data_size_at_origin = data.delivered_data_size_at_origin;
    ack.ttl = M_MAX_TTL;
    ack.ecn_capable_transport = data.ecn_capable_transport;
    ack.congestion_experienced = data.congestion_experienced;

    m_flag_manager.set_flag(ack.flags, m_packet_type_label, PacketType::ACK);
    m_flag_manager.set_flag(ack.flags, m_ack_ttl_label, data.ttl);
    try {
        TimeNs rtt = get_avg_rtt_label(m_flag_manager, data.flags);
        set_avg_rtt_flag(m_flag_manager, ack.flags, rtt);
    } catch (const FlagNotSetException& e) {
        LOG_INFO(
            fmt::format("avg rtt flag does not set in data packet {} so it "
                        "will not be set in "
                        "ack {}",
                        data.to_string(), ack.to_string()));
    }
    return ack;
}

}  // namespace sim
