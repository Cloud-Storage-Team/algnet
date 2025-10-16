#include "tcp_receiver.hpp"

#include "utils/avg_rtt_packet_flag.hpp"

namespace sim {
TcpReceiver::TcpReceiver(TcpCommonPtr a_common) : m_common(a_common) {}

// void TcpReceiver::update(Packet packet) {}

Packet TcpReceiver::create_ack(Packet data) {
    Packet ack;
    ack.packet_num = data.packet_num;
    ack.source_id = m_common->receiver.lock()->get_id();
    ack.dest_id = m_common->sender.lock()->get_id();
    ack.size = SizeByte(1);
    ack.flow_id = m_common->id;
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