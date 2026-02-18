#include "new_tcp_flow.hpp"

#include "metrics/metrics_collector.hpp"
#include "scheduler.hpp"
#include "utils/avg_rtt_packet_flag.hpp"

namespace sim {

std::string NewTcpFlow::m_packet_type_label = "type";

std::shared_ptr<NewTcpFlow> NewTcpFlow::create_shared(
    Id a_id, std::shared_ptr<IHost> a_sender, std::shared_ptr<IHost> a_receiver,
    bool a_ecn_capable, RTO a_rto) {
    return std::shared_ptr<NewTcpFlow>(new NewTcpFlow(std::move(a_id), a_sender,
                                                      a_receiver, a_ecn_capable,
                                                      std::move(a_rto)));
}

void NewTcpFlow::send(std::vector<PacketInfo> packets_info) {
    if (packets_info.empty()) {
        return;
    }
    auto sender = m_context.sender;
    auto receiver = m_context.receiver;
    for (auto info : packets_info) {
        Packet data = create_data_packet(std::move(info), sender, receiver);

        send_data_packet(std::move(data));
    }
}

const FlowContext& NewTcpFlow::get_context() const { return m_context; };

Id NewTcpFlow::get_id() const { return m_id; }

NewTcpFlow::NewTcpFlow(Id a_id, std::shared_ptr<IHost> a_sender,
                       std::shared_ptr<IHost> a_receiver, bool a_ecn_capable,
                       RTO a_rto)
    : m_id(std::move(a_id)),
      m_context({Endpoints{a_sender, a_receiver}, SizeByte(0), SizeByte(0),
                 SizeByte(0),

                 std::nullopt, std::nullopt, utils::Statistics<TimeNs>()}),
      m_ecn_capable(a_ecn_capable),
      m_rto(std::move(a_rto)) {
    if (!m_flag_manager.register_flag_by_amount(m_packet_type_label,
                                                PacketType::ENUM_SIZE)) {
        throw std::runtime_error("Can not registrate packet type label");
    }
    if (!register_packet_avg_rtt_flag(m_flag_manager)) {
        throw std::runtime_error("Can not registrate packet avg rtt label");
    }
}

Packet NewTcpFlow::create_data_packet(PacketInfo info,
                                      std::shared_ptr<IHost> sender,
                                      std::shared_ptr<IHost> receiver) {
    Packet packet;
    packet.packet_num = m_next_packet_num++;

    packet.flags = m_flag_manager;
    packet.flags.set_flag(m_packet_type_label, PacketType::DATA)
        .log_err_if_not_present("Failed to set packet type (DATA)");

    set_avg_rtt_if_present(packet);

    packet.data_id = std::move(info.id);
    packet.source_id = sender->get_id();
    packet.dest_id = receiver->get_id();
    packet.size = info.packet_size;

    packet.flow = nullptr;

    std::weak_ptr<NewTcpFlow> flow_weak = shared_from_this();

    packet.callback = [flow_weak, delivery_callback = info.callback](
                          const Packet& delivered_packet) {
        if (flow_weak.expired()) {
            LOG_ERROR(fmt::format(
                "Can not process data packet {}: flow pointer expired",
                delivered_packet.to_string()));
            return;
        }
        flow_weak.lock()->process_data_packet(delivered_packet,
                                              delivery_callback);
    };
    packet.generated_time = info.generated_time;
    packet.sent_time = Scheduler::get_instance().get_current_time();
    packet.delivered_data_size_at_origin = m_context.delivered_size;

    packet.ecn_capable_transport = m_ecn_capable;
    packet.congestion_experienced = false;

    return packet;
}

void NewTcpFlow::set_avg_rtt_if_present(Packet& packet) {
    std::optional<TimeNs> avg_rtt = m_context.rtt_statistics.get_mean();
    if (avg_rtt.has_value()) {
        set_avg_rtt_flag(packet.flags, avg_rtt.value())
            .log_err_if_not_present("Failed to set average RTT");
    }
}

void NewTcpFlow::send_data_packet(Packet data) {
    TimeNs now = Scheduler::get_instance().get_current_time();

    Scheduler::get_instance().add<Timeout>(now + m_rto.current,
                                           shared_from_this(), data);
    m_context.sent_size += data.size;

    data.sent_time = now;
    m_context.sender->enqueue_packet(std::move(data));
}

void NewTcpFlow::process_data_packet(const Packet& data,
                                     PacketCallback callback) {
    Packet ack = data;
    ack.source_id = m_context.receiver->get_id();
    ack.dest_id = m_context.sender->get_id();
    ack.size = SizeByte(1);
    ack.ttl = M_MAX_TTL;
    ack.flags.set_flag(m_packet_type_label, PacketType::ACK)
        .log_err_if_not_present(
            fmt::format("Flow {}: could not set type label to ack packet {}",
                        m_id, ack.to_string()));
    SizeByte data_packet_size = data.size;

    std::weak_ptr<NewTcpFlow> flow_weak = shared_from_this();
    ack.callback = [flow_weak, callback,
                    data_packet_size](const Packet& delivered_ack) {
        if (flow_weak.expired()) {
            LOG_ERROR(
                fmt::format("Flow pointer expired; could not process ack {}",
                            delivered_ack.to_string()));
            return;
        }
        flow_weak.lock()->process_ack(delivered_ack, data_packet_size,
                                      callback);
    };

    m_context.receiver->enqueue_packet(ack);
}

void NewTcpFlow::process_ack(const Packet& ack, SizeByte data_packet_size,
                             PacketCallback callback) {
    TimeNs now = Scheduler::get_instance().get_current_time();

    // here ack.sent_time is the time when corresponding DATA packet was sent
    // (see process_data_packet)
    TimeNs rtt = now - ack.sent_time;
    m_context.rtt_statistics.add_record(rtt);
    MetricsCollector::get_instance().add_RTT(m_id, now, rtt);

    update_rto_on_ack();

    if (!m_ack_monitor.confirm_one(ack.packet_num)) {
        LOG_WARN(
            fmt::format("Flow {} got ack {} that confirms nothing; ignored",
                        m_id, ack.to_string()));
        return;
    }

    m_context.delivered_size += data_packet_size;

    SpeedGbps delivery_rate =
        (m_context.delivered_size - ack.delivered_data_size_at_origin) /
        (now - ack.generated_time);
    MetricsCollector::get_instance().add_delivery_rate(m_id, now,
                                                       delivery_rate);

    callback({PacketAckInfo{rtt, m_context.rtt_statistics.get_mean().value(),
                            ack.congestion_experienced}});
}

// After ACK with a valid RTT: formula + transition to STEADY (once)
void NewTcpFlow::update_rto_on_ack() {
    auto mean = m_context.rtt_statistics.get_mean().value();
    TimeNs std = m_context.rtt_statistics.get_std().value();
    m_rto.current = std::min(mean * 2 + std * 4, m_rto.max);
    m_rto.is_steady = true;
}

void NewTcpFlow::on_timeout(Packet data) {
    if (m_ack_monitor.is_confirmed(data.packet_num)) {
        LOG_INFO(fmt::format(
            "Flow {}: packet {} is confirmed when timeout reashed; no "
            "retransmit",
            m_id, data.packet_num));
        return;
    }
    update_rto_on_timeout();
    retransmit_packet(std::move(data));
}

// Before the first ACK: exponential growth by timeout
void NewTcpFlow::update_rto_on_timeout() {
    if (!m_rto.is_steady) {
        m_rto.current = std::min(m_rto.current * 2, m_rto.max);
    }
    // in STEADY, don't touch RTO by timeout
}

void NewTcpFlow::retransmit_packet(Packet data) {
    m_context.retransmit_size += data.size;
    send_data_packet(std::move(data));
}

class NewTcpFlow::Timeout : public Event {
public:
    Timeout(TimeNs a_time, std::weak_ptr<NewTcpFlow> a_flow, Packet a_packet)
        : Event(a_time), m_flow(a_flow), m_packet(std::move(a_packet)) {}

    void operator()() {
        if (m_flow.expired()) {
            LOG_ERROR(
                "Could not run TCP flow timout event: flow pointer expired");
            return;
        }
        m_flow.lock()->on_timeout(std::move(m_packet));
    }

private:
    std::weak_ptr<NewTcpFlow> m_flow;
    Packet m_packet;
};

}  // namespace sim
