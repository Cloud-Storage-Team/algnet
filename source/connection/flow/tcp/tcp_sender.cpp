#include "tcp_sender.hpp"

#include "scheduler.hpp"
#include "utils/avg_rtt_packet_flag.hpp"

namespace sim {
TcpSender::TcpSender(TcpCommonPtr a_common, std::unique_ptr<ITcpCC> a_cc,
                     SizeByte a_packet_size)
    : m_common(a_common),
      m_cc(std::move(a_cc)),
      m_packet_size(a_packet_size),
      m_first_send_time(std::nullopt),
      m_last_send_time(std::nullopt),
      m_last_ack_arrive_time(0),
      m_current_rto(2000),
      m_max_rto(Time<Second>(1)),
      m_rto_steady(false),
      m_packets_in_flight(0),
      m_delivered_data_size(0),
      m_next_packet_num(0) {}

void TcpSender::set_avg_rtt_if_present(Packet& packet) {
    std::optional<TimeNs> avg_rtt = m_rtt_statistics.get_mean();
    if (avg_rtt.has_value()) {
        set_avg_rtt_flag(m_common->flag_manager, packet.flags, avg_rtt.value());
    }
}

Packet TcpSender::generate_data_packet(PacketNum packet_num) {
    Packet packet;
    m_common->flag_manager.set_flag(packet.flags, TcpCommon::packet_type_label,
                                    TcpCommon::PacketType::DATA);
    set_avg_rtt_if_present(packet);
    packet.size = m_packet_size;
    packet.flow_id = m_common->id;
    std::weak_ptr<IHost> sender = m_common->sender;
    if (!sender.expired()) {
        packet.source_id = m_common->sender.lock()->get_id();
    } else {
        LOG_ERROR(
            fmt::format("Sender expider for flow {}; sender_id does not set to "
                        "data packet {}",
                        m_common->id, packet.to_string()));
    }
    packet.dest_id = m_common->receiver.lock()->get_id();
    packet.packet_num = packet_num;
    packet.delivered_data_size_at_origin = m_delivered_data_size;
    packet.generated_time = Scheduler::get_instance().get_current_time();
    packet.ttl = TcpCommon::MAX_TTL;
    packet.ecn_capable_transport = m_common->ecn_capable;
    return packet;
}

}  // namespace sim