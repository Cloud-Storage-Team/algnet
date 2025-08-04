#pragma once

#include <type_traits>

#include "device/interfaces/i_host.hpp"
#include "flow/i_flow.hpp"
#include "i_tcp_cc.hpp"
#include "metrics/metrics_collector.hpp"
#include "packet.hpp"
#include "scheduler.hpp"
#include "utils/flag_manager.hpp"
#include "utils/statistics.hpp"

namespace sim {

template <typename TTcpCC>
requires std::derived_from<TTcpCC, ITcpCC>
class TcpFlow : public IFlow,
                public std::enable_shared_from_this<TcpFlow<TTcpCC> > {
public:
    TcpFlow(Id a_id, std::shared_ptr<IHost> a_src,
            std::shared_ptr<IHost> a_dest, TTcpCC a_cc, SizeByte a_packet_size,
            std::uint32_t a_packets_to_send, bool a_ecn_capable = true)
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

    void start() final { send_packets(); }

    void update(Packet packet) final {
        if (packet.dest_id == m_src.lock()->get_id() &&
            m_flag_manager.get_flag(packet, packet_type_label) ==
                PacketType::ACK) {
            // ACK delivered to source device; calculate metrics, update
            // internal state
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

            double old_cwnd = m_cc.get_cwnd();
            m_cc.on_ack(rtt, m_rtt_statistics.get_mean(),
                        packet.congestion_experienced);

            // TODO: get packet size from some other source than
            // m_packet_size (m_data_size does not expand on flows with
            // varying packet sizes)
            m_delivered_data_size += m_packet_size;

            SpeedGbps delivery_rate =
                (m_delivered_data_size - packet.delivered_data_size_at_origin) /
                rtt;
            MetricsCollector::get_instance().add_delivery_rate(
                packet.flow->get_id(), current_time, delivery_rate);

            double cwnd = m_cc.get_cwnd();

            if (old_cwnd != cwnd) {
                MetricsCollector::get_instance().add_cwnd(m_id, current_time,
                                                          cwnd);
            }
        } else if (packet.dest_id == m_dest.lock()->get_id() &&
                   m_flag_manager.get_flag(packet, packet_type_label) ==
                       PacketType::DATA) {
            // data packet delivered to destination device; send ack
            Packet ack(SizeByte(1), this, m_dest.lock()->get_id(),
                       m_src.lock()->get_id(), packet.sent_time,
                       packet.delivered_data_size_at_origin,
                       packet.ecn_capable_transport,
                       packet.congestion_experienced);
            ack.packet_num = packet.packet_num;
            m_flag_manager.set_flag(ack, packet_type_label, PacketType::ACK);
            m_dest.lock()->enqueue_packet(ack);
        }
        send_packets();
    }

    SizeByte get_delivered_data_size() const final {
        return m_delivered_data_size;
    }

    std::shared_ptr<IHost> get_sender() const final { return m_src.lock(); }
    std::shared_ptr<IHost> get_receiver() const { return m_dest.lock(); }
    Id get_id() const final { return m_id; }

    SizeByte get_delivered_bytes() const { return m_delivered_data_size; }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "[TcpFlow; ";
        oss << "Id:" << m_id;
        oss << ", src id: " << m_src.lock()->get_id();
        oss << ", dest id: " << m_dest.lock()->get_id();
        oss << ", CC module: " << m_cc.to_string();
        oss << ", packet size: " << m_packet_size;
        oss << ", to send packets: " << m_packets_to_send;
        oss << ", packets_in_flight: " << m_packets_in_flight;
        oss << ", acked packets: " << m_delivered_data_size;
        oss << "]";
        return oss.str();
    }

private:
    static std::string packet_type_label;
    enum PacketType { ACK, DATA, ENUM_SIZE };

    class SendAtTime : public Event {
    public:
        SendAtTime(TimeNs a_time, std::weak_ptr<TcpFlow<TTcpCC> > a_flow,
                   Packet a_packet)
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
        std::weak_ptr<TcpFlow<TTcpCC> > m_flow;
        Packet m_packet;
    };

    class Timeout : public Event {
    public:
        Timeout(TimeNs a_time, std::weak_ptr<TcpFlow<TTcpCC> > a_flow,
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
            flow->m_cc.on_timeout();
            flow->retransmit_packet(m_packet_num);
        }

    private:
        std::weak_ptr<TcpFlow<TTcpCC> > m_flow;
        PacketNum m_packet_num;
    };

private:
    // Attention: this method DOES NOT set field sent_time to packet
    Packet create_packet(PacketNum packet_num) {
        Packet packet;
        m_flag_manager.set_flag(packet, packet_type_label, PacketType::DATA);
        packet.size = m_packet_size;
        packet.flow = this;
        packet.source_id = get_sender()->get_id();
        packet.dest_id = get_receiver()->get_id();
        packet.packet_num = packet_num;
        packet.delivered_data_size_at_origin = m_delivered_data_size;
        packet.ecn_capable_transport = m_ecn_capable;
        return packet;
    }

    // Attention: this method DOES NOT set field sent_time to packet
    Packet generate_packet() { return create_packet(m_next_packet_num++); }

    TimeNs get_max_timeout() const {
        TimeNs mean = m_rtt_statistics.get_mean();
        TimeNs std = m_rtt_statistics.get_std();
        if (mean == TimeNs(0)) {
            // no measurements
            return TimeNs(std::numeric_limits<long double>::max());
        }
        return mean * 2 + std * 4;
    }

    void send_packet_now(Packet packet) {
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

    // Send (or plan sending) as many packets as possible
    void send_packets() {
        constexpr double EPS = 1e-6;

        TimeNs total_delay(0);
        TimeNs pacing_delay = m_cc.get_pacing_delay();
        TimeNs curr_time = Scheduler::get_instance().get_current_time();

        while (m_packets_to_send > 0 &&
               m_packets_in_flight + 1 < m_cc.get_cwnd() + EPS) {
            Packet packet = generate_packet();
            total_delay += pacing_delay;
            if (pacing_delay == TimeNs(0)) {
                send_packet_now(std::move(packet));
            } else {
                Scheduler::get_instance().add<SendAtTime>(
                    curr_time + total_delay, this->shared_from_this(),
                    std::move(packet));
            }
            m_packets_in_flight++;
            m_packets_to_send--;
        }
    }

    // Retransmits packet with given number
    void retransmit_packet(PacketNum packet_num) {
        Packet packet = create_packet(packet_num);
        send_packet_now(std::move(packet));
    }

    static void initialize_flag_manager() {
        if (!m_is_flag_manager_initialized) {
            m_flag_manager.register_flag_by_amount(packet_type_label,
                                                   PacketType::ENUM_SIZE);
            m_is_flag_manager_initialized = true;
        }
    }

    void set_packet_size(SizeByte size) final {
        m_packet_size = size;
    }

    bool try_send_one() final {
        if (m_packets_to_send > 0 && m_packets_in_flight < m_cc.get_cwnd()) {
            send_packets();
            return true;
        }
        return false;
    }

private:
    const static inline double M_RTT_EXP_DECAY_FACTOR = 0.8;

    static bool m_is_flag_manager_initialized;
    static FlagManager<std::string, PacketFlagsBase> m_flag_manager;

    Id m_id;

    std::weak_ptr<IHost> m_src;
    std::weak_ptr<IHost> m_dest;

    // Congestion control module
    TTcpCC m_cc;

    SizeByte m_packet_size;
    std::uint32_t m_packets_to_send;
    bool m_ecn_capable;

    std::uint32_t m_packets_in_flight;
    SizeByte m_delivered_data_size;
    PacketNum m_next_packet_num;

    // Contains numbers of all delivered acks
    std::set<PacketNum> m_acked;

    utils::Statistics<TimeNs> m_rtt_statistics;
};

template <typename TTcpCC>
requires std::derived_from<TTcpCC, ITcpCC>
std::string TcpFlow<TTcpCC>::packet_type_label = "type";

template <typename TTcpCC>
requires std::derived_from<TTcpCC, ITcpCC>
FlagManager<std::string, PacketFlagsBase> TcpFlow<TTcpCC>::m_flag_manager;

template <typename TTcpCC>
requires std::derived_from<TTcpCC, ITcpCC>
bool TcpFlow<TTcpCC>::m_is_flag_manager_initialized = false;

}  // namespace sim
