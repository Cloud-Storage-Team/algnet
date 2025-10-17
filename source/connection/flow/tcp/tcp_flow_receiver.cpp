#include "tcp_flow_receiver.hpp"

#include "metrics/metrics_collector.hpp"
#include "scheduler.hpp"
#include "utils/avg_rtt_packet_flag.hpp"

namespace sim {
TcpFlowReceiver::TcpFlowReceiver(TcpCommonPtr a_common) : m_common(a_common) {}

void TcpFlowReceiver::process_data_packet(Packet packet) {
    if (m_common->flag_manager.get_flag(packet.flags,
                                        m_common->packet_type_label) !=
        TcpFlowCommon::PacketType::DATA) {
        LOG_ERROR(
            fmt::format("Called TcpFlowReceiver::update with packet {}, but "
                        "its type differs "
                        "from data; ignored",
                        packet.to_string()));
        return;
    }
    m_data_packets_monitor.confirm_one(packet.packet_num);
    TimeNs current_time = Scheduler::get_instance().get_current_time();
    m_packet_reordering.add_record(packet.packet_num);
    MetricsCollector::get_instance().add_packet_reordering(
        m_common->id, current_time, m_packet_reordering.value());

    Packet ack = create_ack(std::move(packet));

    m_common->receiver.lock()->enqueue_packet(ack);
}

Packet TcpFlowReceiver::create_ack(Packet data) {
    Packet ack;
    std::optional<PacketNum> last_confirmed =
        m_data_packets_monitor.get_last_confirmed();
    if (!last_confirmed) {
        LOG_ERROR(
            "Can not correcly calculate ACK numer: there are no packets "
            "confirmed; ack number set to 0");
        ack.packet_num = 0;
    } else {
        ack.packet_num = last_confirmed.value();
    }
    ack.source_id = m_common->receiver.lock()->get_id();
    ack.dest_id = m_common->sender.lock()->get_id();
    ack.size = SizeByte(1);
    ack.flow_id = m_common->id;
    ack.generated_time = data.generated_time;
    ack.sent_time = data.sent_time;
    ack.delivered_data_size_at_origin = data.delivered_data_size_at_origin;
    ack.ttl = TcpFlowCommon::MAX_TTL;
    ack.ecn_capable_transport = data.ecn_capable_transport;
    ack.congestion_experienced = data.congestion_experienced;

    m_common->flag_manager.set_flag(ack.flags, TcpFlowCommon::packet_type_label,
                                    TcpFlowCommon::PacketType::ACK);
    m_common->flag_manager.set_flag(ack.flags, TcpFlowCommon::ack_ttl_label,
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

}  // namespace sim