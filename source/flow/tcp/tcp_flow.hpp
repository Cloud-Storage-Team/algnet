#pragma once

#include <type_traits>

#include "device/interfaces/i_host.hpp"
#include "flow/i_flow.hpp"
#include "i_tcp_cc.hpp"
#include "metrics/metrics_collector.hpp"
#include "packet.hpp"
#include "scheduler.hpp"
#include "utils/flag_manager.hpp"

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
          m_delivered(0),
          m_sent_bytes(0),
          m_avg_rtt(0.0) {
        if (m_src.lock() == nullptr) {
            throw std::invalid_argument("Sender for TcpFlow is nullptr");
        }
        if (m_dest.lock() == nullptr) {
            throw std::invalid_argument("Receiver for TcpFlow is nullptr");
        }
        initialize_flag_manager();
    }

    void start() final { send_packets(); }

    void update(Packet packet, DeviceType type) final {
        (void)type;
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

            TimeNs rtt = current_time - packet.sent_time;
            update_avg_rtt(rtt);
            MetricsCollector::get_instance().add_RTT(packet.flow->get_id(),
                                                     current_time, rtt);

            double old_cwnd = m_cc.get_cwnd();

            if (m_packets_in_flight > 0) {
                m_packets_in_flight--;
            }
            if (!m_cc.on_ack(rtt, m_avg_rtt, packet.congestion_experienced)) {
                // No congestion
                m_delivered += m_packet_size;
            }

            SpeedGbps delivery_rate =
                (m_delivered - packet.delivered_at_origin) / rtt;
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
                       packet.delivered_at_origin, packet.ecn_capable_transport,
                       packet.congestion_experienced);
            m_flag_manager.set_flag(ack, packet_type_label, PacketType::ACK);
            m_dest.lock()->enqueue_packet(ack);
        }
        send_packets();
    }

    std::shared_ptr<IHost> get_sender() const final { return m_src.lock(); }
    std::shared_ptr<IHost> get_receiver() const { return m_dest.lock(); }
    Id get_id() const final { return m_id; }

    SizeByte get_delivered_bytes() const { return m_delivered; }

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
        oss << ", acked packets: " << m_delivered;
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

    // Attention: this method DOES NOT set field sent_time to packet
    Packet generate_packet() {
        sim::Packet packet;
        m_flag_manager.set_flag(packet, packet_type_label, PacketType::DATA);
        packet.size = m_packet_size;
        packet.flow = this;
        packet.source_id = get_sender()->get_id();
        packet.dest_id = get_receiver()->get_id();
        packet.delivered_at_origin = m_delivered;
        packet.ecn_capable_transport = m_ecn_capable;
        return packet;
    }

    void send_packet_now(Packet packet) {
        // TODO: think about this place(should be here or in send_packets)
        m_sent_bytes += packet.size;
        packet.sent_time = Scheduler::get_instance().get_current_time();
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

    static void initialize_flag_manager() {
        if (!m_is_flag_manager_initialized) {
            m_flag_manager.register_flag_by_amount(packet_type_label,
                                                   PacketType::ENUM_SIZE);
            m_is_flag_manager_initialized = true;
        }
    }

    void update_avg_rtt(TimeNs rtt) {
        if (m_avg_rtt == TimeNs(0.0)) {
            // If not initialized before
            m_avg_rtt = rtt;
        } else {
            m_avg_rtt = m_avg_rtt * M_RTT_WEIGHT_DECAY_FACTOR +
                        rtt * (1 - M_RTT_WEIGHT_DECAY_FACTOR);
        }
    }

private:
    const static inline double M_RTT_WEIGHT_DECAY_FACTOR = 0.8;

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
    SizeByte m_delivered;
    SizeByte m_sent_bytes;

    TimeNs m_avg_rtt;
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
