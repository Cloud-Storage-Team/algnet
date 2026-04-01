#include "tcp_flow.hpp"

#include "flow_metrics_metadatas.hpp"
#include "scheduler/scheduler.hpp"
#include "utils/avg_rtt_packet_flag.hpp"

namespace sim {

std::string TcpFlow::m_packet_type_label = "type";

std::shared_ptr<TcpFlow> TcpFlow::create_shared(
    Id a_id, FlowFourTuple a_four_tuple, bool a_ecn_capable, RTO a_rto,
    TcpFlowMetricsFilters a_metrics_flags) {
    return std::shared_ptr<TcpFlow>(
        new TcpFlow(std::move(a_id), std::move(a_four_tuple), a_ecn_capable,
                    std::move(a_rto), std::move(a_metrics_flags)));
}

void TcpFlow::send(std::vector<PacketInfo> packets_info) {
    if (packets_info.empty()) {
        return;
    }
    if (!m_context.start_time.has_value()) {
        m_context.start_time = Scheduler::get_instance().get_current_time();
    }
    auto sender = m_context.sender;
    auto receiver = m_context.receiver;
    for (auto info : packets_info) {
        send_data_packet(create_data_packet(std::move(info), sender, receiver));
    }
}

const FlowContext& TcpFlow::get_context() const { return m_context; };

const Id& TcpFlow::get_id() const { return m_id; }

MetricsTable TcpFlow::get_metrics_table() const {
    MetricsTable metrics_table;
    auto add_if = [&](bool cond, const MetricId& metric_id,
                      std::shared_ptr<MetricsStorage> metric_storage) {
        if (cond) {
            metrics_table.emplace(metric_id, metric_storage);
        }
    };
    add_if(m_metrics_filters.rtt, FlowMetricMetadatas::RTT, m_metrics.rtt);
    add_if(m_metrics_filters.delivery_rate, FlowMetricMetadatas::DELIVERY_RATE,
           m_metrics.delivery_rate);
    add_if(m_metrics_filters.reordering, FlowMetricMetadatas::PACKET_REORDERING,
           m_metrics.packet_reordering);
    return metrics_table;
}

void TcpFlow::write_inner_metrics(
    [[maybe_unused]] std::filesystem::path output_dir) const {};

TcpFlow::TcpFlow(Id a_id, FlowFourTuple a_four_tuple, bool a_ecn_capable,
                 RTO a_rto, TcpFlowMetricsFilters a_metrics_flags)
    : m_id(std::move(a_id)),
      m_context(a_four_tuple),
      m_ecn_capable(a_ecn_capable),
      m_rto(std::move(a_rto)),
      m_metrics({std::make_shared<MetricsStorage>(),
                 std::make_shared<MetricsStorage>(),
                 std::make_shared<MetricsStorage>()}),
      m_metrics_filters(std::move(a_metrics_flags)) {
    if (!m_flag_manager.register_flag_by_amount(m_packet_type_label,
                                                PacketType::ENUM_SIZE)) {
        throw std::runtime_error("Can not register packet type label");
    }
    if (!register_packet_avg_rtt_flag(m_flag_manager)) {
        throw std::runtime_error("Can not register packet avg rtt label");
    }
}

Packet TcpFlow::create_data_packet(PacketInfo info,
                                   std::shared_ptr<IHost> sender,
                                   std::shared_ptr<IHost> receiver) {
    Packet packet;
    packet.packet_num = m_next_packet_num++;

    packet.flags = m_flag_manager;
    packet.flags.set_flag(m_packet_type_label, PacketType::DATA)
        .log_err_if_not_present("Failed to set packet type (DATA)");

    set_avg_rtt_if_present(packet);

    packet.data_id = std::move(info.id);
    packet.sender_id = sender->get_id();
    packet.sender_port = m_context.sender_port;
    packet.receiver_port = m_context.receiver_port;
    packet.receiver_id = receiver->get_id();
    packet.size = info.packet_size;

    std::shared_ptr<TcpFlow> flow = shared_from_this();

    packet.callback = [flow, delivery_callback = info.callback](
                          const Packet& delivered_packet) {
        flow->process_data_packet(delivered_packet, delivery_callback);
    };
    packet.generated_time = info.generated_time;
    packet.sent_time = Scheduler::get_instance().get_current_time();
    packet.delivered_data_size_at_origin = m_context.delivered_size;

    packet.ecn_capable_transport = m_ecn_capable;
    packet.congestion_experienced = false;

    return packet;
}

void TcpFlow::set_avg_rtt_if_present(Packet& packet) {
    std::optional<TimeNs> avg_rtt = m_context.rtt_statistics.get_mean();
    if (avg_rtt.has_value()) {
        set_avg_rtt_flag(packet.flags, avg_rtt.value())
            .log_err_if_not_present("Failed to set average RTT");
    }
}

void TcpFlow::send_data_packet(Packet data) {
    TimeNs now = Scheduler::get_instance().get_current_time();

    Scheduler::get_instance().add(
        now + m_rto.current,
        [flow = shared_from_this(), data]() { flow->on_timeout(data); });
    m_context.sent_size += data.size;

    data.sent_time = now;
    m_context.sender->enqueue_packet(data);
}

void TcpFlow::process_data_packet(const Packet& data,
                                  const PacketCallback& callback) {
    m_packet_reordering.add_record(data.packet_num);
    m_metrics.packet_reordering->add_record(
        Scheduler::get_instance().get_current_time(),
        m_packet_reordering.value());
    Packet ack = data;
    ack.sender_id = IdWithHash(m_context.receiver->get_id());
    ack.sender_port = m_context.receiver_port;
    ack.receiver_id = IdWithHash(m_context.sender->get_id());
    ack.receiver_port = m_context.sender_port;
    ack.size = SizeByte(1);
    ack.ttl = M_MAX_TTL;
    auto exp_void = ack.flags.set_flag(m_packet_type_label, PacketType::ACK);
    if (!exp_void.has_value()) {
        LOG_ERROR(
            fmt::format("Flow {}: could not set type label to ack packet {}",
                        m_id, ack.to_string()));
    }
    SizeByte data_packet_size = data.size;

    std::shared_ptr<TcpFlow> flow = shared_from_this();
    ack.callback = [flow, callback,
                    data_packet_size](const Packet& delivered_ack) {
        flow->process_ack(delivered_ack, data_packet_size, callback);
    };

    m_context.receiver->enqueue_packet(ack);
}

void TcpFlow::process_ack(const Packet& ack, SizeByte data_packet_size,
                          PacketCallback callback) {
    TimeNs now = Scheduler::get_instance().get_current_time();

    // here ack.sent_time is the time when corresponding DATA packet was sent
    // (see process_data_packet)
    TimeNs rtt = now - ack.sent_time;
    m_context.rtt_statistics.add_record(rtt);

    m_metrics.rtt->add_record(now, rtt.value());

    update_rto_on_ack();

    if (!m_ack_monitor.confirm_one(ack.packet_num)) {
        LOG_WARN(
            fmt::format("Flow {} got ack {} that confirms nothing; ignored",
                        m_id, ack.to_string()));
        return;
    }
    m_context.last_ack_receive_time = now;

    m_context.delivered_size += data_packet_size;

    SpeedGbps delivery_rate =
        (m_context.delivered_size - ack.delivered_data_size_at_origin) /
        (now - ack.generated_time);

    m_metrics.delivery_rate->add_record(now, delivery_rate.value());
    m_context.delivery_rate_statistics.add_record(delivery_rate);

    callback({PacketAckInfo{rtt, m_context.rtt_statistics.get_mean().value(),
                            ack.congestion_experienced}});
}

// After ACK with a valid RTT: formula + transition to STEADY (once)
void TcpFlow::update_rto_on_ack() {
    auto mean = m_context.rtt_statistics.get_mean().value();
    TimeNs std = m_context.rtt_statistics.get_std().value();
    m_rto.current = std::min(mean * 2 + std * 4, m_rto.max);
    m_rto.is_steady = true;
}

void TcpFlow::on_timeout(const Packet& data) {
    if (m_ack_monitor.is_confirmed(data.packet_num)) {
        LOG_INFO(fmt::format(
            "Flow {}: packet {} is confirmed when timeout reached; no "
            "retransmit",
            m_id, data.packet_num));
        return;
    }
    update_rto_on_timeout();
    retransmit_packet(data);
}

// Before the first ACK: exponential growth by timeout
void TcpFlow::update_rto_on_timeout() {
    if (!m_rto.is_steady) {
        m_rto.current = std::min(m_rto.current * 2, m_rto.max);
    }
    // in STEADY, don't touch RTO by timeout
}

void TcpFlow::retransmit_packet(const Packet& data) {
    m_context.retransmit_size += data.size;
    send_data_packet(data);
}

}  // namespace sim
