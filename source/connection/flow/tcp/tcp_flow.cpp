#include "connection/flow/tcp/tcp_flow.hpp"

#include "connection/i_connection.hpp"
#include "metrics/metrics_collector.hpp"
#include "packet.hpp"
#include "scheduler.hpp"
#include "utils/avg_rtt_packet_flag.hpp"

namespace sim {

TcpFlow::TcpFlow(Id a_id, std::shared_ptr<IConnection> a_conn,
                 std::unique_ptr<ITcpCC> a_cc, SizeByte a_packet_size,
                 bool a_ecn_capable)
    : m_common(std::make_shared<TcpCommon>(
          std::move(a_id), a_conn, a_conn->get_sender(), a_conn->get_receiver(),
          a_ecn_capable)),
      m_sender(TcpSender::create(m_common, std::move(a_cc), a_packet_size)),
      m_receiver(m_common) {}

SizeByte TcpFlow::get_delivered_data_size() const {
    return m_sender->get_delivered_data_size();
}

std::optional<TimeNs> TcpFlow::get_fct() const { return m_sender->get_fct(); }

const BaseFlagManager& TcpFlow::get_flag_manager() const {
    return m_common->flag_manager;
}

std::shared_ptr<IHost> TcpFlow::get_sender() const {
    return m_common->sender.lock();
}

std::shared_ptr<IHost> TcpFlow::get_receiver() const {
    return m_common->receiver.lock();
}

Id TcpFlow::get_id() const { return m_common->id; }

SizeByte TcpFlow::get_delivered_bytes() const {
    return m_sender->get_delivered_data_size();
}

SizeByte TcpFlow::get_sending_quota() const {
    return m_sender->get_sending_quota();
}

void TcpFlow::send_data(SizeByte data) { return m_sender->send_data(data); }

void TcpFlow::update(Packet packet) {
    TcpCommon::PacketType type =
        static_cast<TcpCommon::PacketType>(m_common->flag_manager.get_flag(
            packet.flags, TcpCommon::packet_type_label));
    TimeNs current_time = Scheduler::get_instance().get_current_time();
    if (packet.dest_id == m_common->sender.lock()->get_id() &&
        type == TcpCommon::PacketType::ACK) {
        m_sender->update(std::move(packet));

        if (m_common->connection.expired()) {
            LOG_ERROR(fmt::format(
                "Can not update coinnection from flow '{}': connection expired",
                m_common->id));
        } else {
            m_common->connection.lock()->update(shared_from_this());
        }
    } else if (packet.dest_id == m_common->receiver.lock()->get_id() &&
               type == TcpCommon::PacketType::DATA) {
        m_receiver.m_packet_reordering.add_record(packet.packet_num);
        MetricsCollector::get_instance().add_packet_reordering(
            m_common->id, current_time, m_receiver.m_packet_reordering.value());

        Packet ack = m_receiver.create_ack(std::move(packet));

        m_common->receiver.lock()->enqueue_packet(ack);
    }
}

std::optional<TimeNs> TcpFlow::get_last_rtt() const {
    return m_sender->get_last_rtt();
}

std::string TcpFlow::to_string() const {
    std::ostringstream oss;
    oss << "Id:" << m_common->id;
    oss << ", src id: " << m_common->sender.lock()->get_id();
    oss << ", dest id: " << m_common->receiver.lock()->get_id();
    oss << "sender: [" << m_sender->to_string() << ']';
    return oss.str();
}

}  // namespace sim
