#include "flow/tcp/tcp_flow.hpp"

#include "connection/connection.hpp"

namespace sim {

std::string TcpFlow::m_packet_type_label = "type";
FlagManager<std::string, PacketFlagsBase> TcpFlow::m_flag_manager;
bool TcpFlow::m_is_flag_manager_initialized = false;

TcpFlow::TcpFlow(Id a_id, std::shared_ptr<IHost> a_src,
                 std::shared_ptr<IHost> a_dest, std::unique_ptr<ITcpCC> a_cc,
                 SizeByte a_packet_size, std::uint32_t a_packets_to_send,
                 bool a_ecn_capable)
    : m_id(std::move(a_id)),
      m_src(a_src),
      m_dest(a_dest),
      m_cc(std::move(a_cc)),
      m_packet_size(a_packet_size),
      m_packets_to_send(a_packets_to_send),
      m_ecn_capable(a_ecn_capable),
      m_packets_in_flight(0),
      m_delivered_data_size(0),
      m_next_packet_num(0),
      m_rtt_statistics(M_RTT_EXP_DECAY_FACTOR) {
    if (m_src.lock() == nullptr) {
        throw std::invalid_argument("Sender for TcpFlow is nullptr");
    }
    if (m_dest.lock() == nullptr) {
        throw std::invalid_argument("Receiver for TcpFlow is nullptr");
    }
    initialize_flag_manager();
}

void TcpFlow::start() {
    if (!m_using_connection) {
        send_packets();
    }
}

SizeByte TcpFlow::get_delivered_data_size() const {
    return m_delivered_data_size;
}

std::shared_ptr<IHost> TcpFlow::get_sender() const { return m_src.lock(); }

std::shared_ptr<IHost> TcpFlow::get_receiver() const { return m_dest.lock(); }

Id TcpFlow::get_id() const { return m_id; }

SizeByte TcpFlow::get_delivered_bytes() const { return m_delivered_data_size; }

bool TcpFlow::can_send() const {
    constexpr double EPS = 1e-6;
    return m_packets_in_flight + 1 < m_cc->get_cwnd() + EPS;
}

Packet TcpFlow::create_packet(PacketNum packet_num) {
    Packet packet;
    m_flag_manager.set_flag(packet, m_packet_type_label, PacketType::DATA);
    packet.size = m_packet_size;
    packet.flow = this;
    packet.source_id = get_sender()->get_id();
    packet.dest_id = get_receiver()->get_id();
    packet.packet_num = packet_num;
    packet.delivered_data_size_at_origin = m_delivered_data_size;
    packet.ecn_capable_transport = m_ecn_capable;
    return packet;
}

void TcpFlow::send_packet() {
    Packet packet = create_packet(m_next_packet_num++);
    TimeNs pacing_delay = m_cc->get_pacing_delay();
    TimeNs now = Scheduler::get_instance().get_current_time();

    if (pacing_delay == TimeNs(0)) {
        send_packet_now(std::move(packet));
    } else {
        Scheduler::get_instance().add<SendAtTime>(
            now + pacing_delay, this->shared_from_this(), std::move(packet));
    }

    m_packets_in_flight++;
}

void TcpFlow::set_conn(std::shared_ptr<Connection> connection) {
    m_connection = std::move(connection);
    m_using_connection = true;
}

std::shared_ptr<Connection> TcpFlow::get_conn() const { return m_connection; }

Packet TcpFlow::generate_packet() { return create_packet(m_next_packet_num++); }

void TcpFlow::initialize_flag_manager() {
    if (!m_is_flag_manager_initialized) {
        m_flag_manager.register_flag_by_amount(m_packet_type_label,
                                               PacketType::ENUM_SIZE);
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

        if (flow->m_acked.contains(m_packet_num)) {
            return;
        }
        LOG_WARN(
            fmt::format("Timeout for packet number {} expired; looks "
                        "like packet loss",
                        m_packet_num));
        flow->m_cc->on_timeout();
        flow->retransmit_packet(m_packet_num);
    }

private:
    std::weak_ptr<TcpFlow> m_flow;
    PacketNum m_packet_num;
};

void TcpFlow::update(Packet packet) {
    PacketType type = static_cast<PacketType>(
        m_flag_manager.get_flag(packet, m_packet_type_label));
    if (packet.dest_id == m_src.lock()->get_id() && type == PacketType::ACK) {
        TimeNs current_time = Scheduler::get_instance().get_current_time();
        if (current_time < packet.sent_time) {
            LOG_ERROR("Packet " + packet.to_string() +
                      " current time less that sending time; ignored");
            return;
        }

        if (m_acked.contains(packet.packet_num)) {
            LOG_WARN(fmt::format("Got duplicate ack number {}; ignored",
                                 packet.packet_num));
            return;
        }

        TimeNs rtt = current_time - packet.sent_time;
        m_rtt_statistics.add_record(rtt);
        MetricsCollector::get_instance().add_RTT(packet.flow->get_id(),
                                                 current_time, rtt);
        m_acked.insert(packet.packet_num);

        if (m_packets_in_flight > 0) {
            m_packets_in_flight--;
        }

        double old_cwnd = m_cc->get_cwnd();
        m_cc->on_ack(rtt, m_rtt_statistics.get_mean(),
                     packet.congestion_experienced);

        m_delivered_data_size += m_packet_size;

        SpeedGbps delivery_rate =
            (m_delivered_data_size - packet.delivered_data_size_at_origin) /
            rtt;
        MetricsCollector::get_instance().add_delivery_rate(
            packet.flow->get_id(), current_time, delivery_rate);

        double cwnd = m_cc->get_cwnd();
        if (old_cwnd != cwnd) {
            MetricsCollector::get_instance().add_cwnd(m_id, current_time, cwnd);
        }
        if (m_using_connection) {
            FlowSample sample{.ack_recv_time = current_time,
                              .packet_sent_time = packet.sent_time,
                              .packets_in_flight = m_packets_in_flight,
                              .curr_cwnd = cwnd,
                              .delivery_rate = delivery_rate};
            m_connection->update(shared_from_this(), sample);
        }
    } else if (packet.dest_id == m_dest.lock()->get_id() &&
               type == PacketType::DATA) {
        Packet ack(SizeByte(1), this, m_dest.lock()->get_id(),
                   m_src.lock()->get_id(), packet.sent_time,
                   packet.delivered_data_size_at_origin,
                   packet.ecn_capable_transport, packet.congestion_experienced);
        ack.packet_num = packet.packet_num;
        m_flag_manager.set_flag(ack, m_packet_type_label, PacketType::ACK);
        m_dest.lock()->enqueue_packet(ack);
    }
    if (!m_using_connection) {
        send_packets();
    }
}

std::string TcpFlow::to_string() const {
    std::ostringstream oss;
    oss << "[TcpFlow; ";
    oss << "Id:" << m_id;
    oss << ", src id: " << m_src.lock()->get_id();
    oss << ", dest id: " << m_dest.lock()->get_id();
    oss << ", CC module: " << m_cc->to_string();
    oss << ", packet size: " << m_packet_size;
    oss << ", to send packets: " << m_packets_to_send;
    oss << ", packets_in_flight: " << m_packets_in_flight;
    oss << ", acked packets: " << m_delivered_data_size;
    oss << "]";
    return oss.str();
}

TimeNs TcpFlow::get_max_timeout() const {
    TimeNs mean = m_rtt_statistics.get_mean();
    TimeNs std = m_rtt_statistics.get_std();
    if (mean == TimeNs(0)) {
        return TimeNs(std::numeric_limits<long double>::max());
    }
    return mean * 2 + std * 4;
}

void TcpFlow::send_packet_now(Packet packet) {
    TimeNs current_time = Scheduler::get_instance().get_current_time();

    TimeNs max_timeout = get_max_timeout();
    if (max_timeout != TimeNs(std::numeric_limits<long double>::max())) {
        Scheduler::get_instance().add<Timeout>(current_time + max_timeout,
                                               this->shared_from_this(),
                                               packet.packet_num);
    }

    packet.sent_time = current_time;
    m_src.lock()->enqueue_packet(std::move(packet));
}

void TcpFlow::send_packets() {
    constexpr double EPS = 1e-6;
    TimeNs total_delay(0);
    TimeNs pacing_delay = m_cc->get_pacing_delay();
    TimeNs curr_time = Scheduler::get_instance().get_current_time();

    while (m_packets_to_send > 0 &&
           m_packets_in_flight + 1 < m_cc->get_cwnd() + EPS) {
        Packet packet = generate_packet();
        total_delay += pacing_delay;
        if (pacing_delay == TimeNs(0)) {
            send_packet_now(std::move(packet));
        } else {
            Scheduler::get_instance().add<SendAtTime>(curr_time + total_delay,
                                                      this->shared_from_this(),
                                                      std::move(packet));
        }
        m_packets_in_flight++;
        m_packets_to_send--;
    }
}

void TcpFlow::retransmit_packet(PacketNum packet_num) {
    Packet packet = create_packet(packet_num);
    send_packet_now(std::move(packet));
}

}  // namespace sim
