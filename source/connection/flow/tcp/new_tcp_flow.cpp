#include "new_tcp_flow.hpp"

#include "metrics/metrics_collector.hpp"
#include "scheduler.hpp"
#include "utils/avg_rtt_packet_flag.hpp"

namespace sim {

std::string NewTcpFlow::m_packet_type_label = "type";
std::string NewTcpFlow::m_ack_ttl_label = "ack_ttl";

std::shared_ptr<NewTcpFlow> NewTcpFlow::create_shared(
    Id a_id, std::shared_ptr<IHost> a_sender, std::shared_ptr<IHost> a_receiver,
    bool a_ecn_capable) {
    return std::shared_ptr<NewTcpFlow>(
        new NewTcpFlow(std::move(a_id), a_sender, a_receiver, a_ecn_capable));
}

void NewTcpFlow::send(std::vector<PacketInfo> packets_info) {
    if (packets_info.empty()) {
        return;
    }
    auto exp_endpoints = get_endpoints();
    if (!exp_endpoints.has_value()) {
        LOG_ERROR(fmt::format("Flow {}; error while sending packets: {}", m_id,
                              exp_endpoints.error()));
        return;
    }
    auto [sender, receiver] = exp_endpoints.value();
    for (auto info : packets_info) {
        Packet data = create_data_packet(std::move(info), sender, receiver);

        sender->enqueue_packet(data);
    }
}

const FlowContext& NewTcpFlow::get_context() const { return m_context; };

Id NewTcpFlow::get_id() const { return m_id; }

NewTcpFlow::NewTcpFlow(Id a_id, std::shared_ptr<IHost> a_sender,
                       std::shared_ptr<IHost> a_receiver, bool a_ecn_capable)
    : m_id(std::move(a_id)),
      m_ecn_capable(a_ecn_capable),
      m_context({SizeByte(0), SizeByte(0), SizeByte(0),

                 std::nullopt, std::nullopt, utils::Statistics<TimeNs>(),

                 a_sender, a_receiver}) {
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
}

utils::StrExpected<NewTcpFlow::Endpoints> NewTcpFlow::get_endpoints() const {
    if (m_context.sender.expired()) {
        return std::unexpected(fmt::format("sender pointer expired", m_id));
    }

    if (m_context.receiver.expired()) {
        return std::unexpected(fmt::format("receiver pointer expired", m_id));
    }
    return Endpoints{m_context.sender.lock(), m_context.receiver.lock()};
}

Packet NewTcpFlow::create_data_packet(PacketInfo info,
                                      std::shared_ptr<IHost> sender,
                                      std::shared_ptr<IHost> receiver) {
    Packet packet;
    packet.packet_num = m_next_packet_num++;

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

void NewTcpFlow::process_data_packet(const Packet& data,
                                     PacketCallback callback) {
    auto exp_endpoints = get_endpoints();
    if (!exp_endpoints.has_value()) {
        LOG_ERROR(
            fmt::format("Flow {}: error while processing data packet {}: {}",
                        m_id, data.to_string(), exp_endpoints.error()));
        return;
    }
    auto [sender, receiver] = exp_endpoints.value();

    Packet ack = data;
    ack.source_id = receiver->get_id();
    ack.dest_id = sender->get_id();
    ack.size = SizeByte(1);
    ack.ttl = M_MAX_TTL;
    SizeByte data_packet_size = data.size;

    std::weak_ptr<NewTcpFlow> flow_weak = shared_from_this();
    ack.callback = [flow_weak, callback,
                    data_packet_size](const Packet& delivered_ack) {
        if (flow_weak.expired()) {
            LOG_ERROR(
                fmt::format("Flow pointer expired; could not process ack {}",
                            delivered_ack.to_string()));
            std::shared_ptr<NewTcpFlow> flow = flow_weak.lock();
            flow->process_ack(delivered_ack, data_packet_size, callback);
        }
    };

    receiver->enqueue_packet(ack);
}

void NewTcpFlow::process_ack(const Packet& ack, SizeByte data_packet_size,
                             PacketCallback callback) {
    TimeNs now = Scheduler::get_instance().get_current_time();
    TimeNs rtt = now - ack.sent_time;
    m_context.rtt_statistics.add_record(rtt);
    // TODO: update RTO

    MetricsCollector::get_instance().add_RTT(m_id, now, rtt);

    m_context.delivered_size += data_packet_size;

    SpeedGbps delivery_rate =
        (m_context.delivered_size - ack.delivered_data_size_at_origin) /
        (now - ack.generated_time);
    MetricsCollector::get_instance().add_delivery_rate(ack.flow->get_id(), now,
                                                       delivery_rate);

    callback({PacketAckInfo{rtt, m_context.rtt_statistics.get_mean().value(),
                            ack.congestion_experienced}});
}

}  // namespace sim
