#include "rdma_connectrion.hpp"
#include "scheduler/scheduler.hpp"
#include "utils/defer.hpp"

namespace sim {

RdmaConnectionPtr RdmaConnection::create_shared(const RdmaParams& a_params) {
    return RdmaConnectionPtr(new RdmaConnection(a_params));
}

[[nodiscard]] utils::StrExpected<void> RdmaConnection::send_data(
    Data data, OnDeliveryCallback callback) {
    DataId data_id = data.id;
    if (m_data_context_table.contains(data_id)) {
        return std::unexpected(
            fmt::format("Connection {}: could not add data {}: data with given "
                        "id already sent",
                        m_id, data.to_string()));
    }
    if (m_ctx.first_send_data_time == std::nullopt) {
        m_ctx.first_send_data_time =
            Scheduler::get_instance().get_current_time();
    }
    m_data_context_table[data_id] =
        DataContext{data.size, SizeByte(0ul), SizeByte(0ul), callback};
    m_ctx.total_data_added += data.size;

    SizeByte total_size(0ul);
    if (data.size == SizeByte(0ul)) {
        return std::unexpected(fmt::format(
            "Rdma connection {}: could not send data with zero size", m_id));
    }
    bool was_sending_queue_empty = m_send_queue.empty();
    while (total_size < data.size) {
        m_send_queue.emplace_back(create_data_packet(data));
        total_size += m_packet_size;
    }
    if (was_sending_queue_empty) {
        start_data_sending();
    }
    return {};
}

ConnectionContext RdmaConnection::get_context() const { return m_ctx; }

MetricsTable RdmaConnection::get_metrics_table() const {
    return MetricsTable();
}

void RdmaConnection::write_inner_metrics(
    [[maybe_unused]] std::filesystem::path output_dir) const {}

const Id& RdmaConnection::get_id() const { return m_id; }

RdmaConnection::RdmaConnection(const RdmaParams& a_params)
    : m_id(a_params.id),
      m_dcqcn(a_params.dcqcn),
      m_sender(a_params.ft.sender),
      m_sender_port(a_params.ft.sender_port),
      m_packet_size(a_params.packet_size),
      m_receiver(a_params.ft.receiver),
      m_receiver_port(a_params.ft.receiver_port),
      m_ack_threshold(a_params.ack_threshold),
      m_max_reorder_buffer_size(a_params.reorder_buffer_size / m_packet_size) {}

void RdmaConnection::start_data_sending() {
    m_dcqcn.start();
    schedule_data_send();
}

void RdmaConnection::schedule_data_send() {
    Scheduler& sched = Scheduler::get_instance();
    TimeNs now = sched.get_current_time();

    SpeedGbps rate = m_dcqcn.get_rate();
    TimeNs delay = m_packet_size / rate;
    RdmaConnectionPtr conn = shared_from_this();
    sched.add(now + delay, [conn]() { conn->send_next_data_packet(); });
}

void RdmaConnection::send_next_data_packet() {
    uint32_t i = m_pcn - m_last_acked_pcn - 1;
    if (i >= m_send_queue.size()) {
        LOG_INFO(
            fmt::format("RDMA connection {}: send all packet from sending "
                        "queue; send ack request packet",
                        m_id));
        send_ack_request();
        return;
    }
    m_pcn++;
    const Packet& data = m_send_queue[i];
    m_sender->enqueue_packet(data);
    schedule_data_send();
}

Packet RdmaConnection::create_data_packet(const Data& data) {
    Packet packet;
    packet.packet_num = m_next_packet_num++;

    packet.data_id = std::move(data.id);
    packet.sender_id = m_sender->get_id();
    packet.sender_port = m_sender_port;
    packet.receiver_port = m_receiver_port;
    packet.receiver_id = m_receiver->get_id();
    packet.size = m_packet_size;

    RdmaConnectionPtr conn = shared_from_this();

    packet.callback = [conn](const Packet& delivered_packet) {
        conn->process_data_packet(delivered_packet);
    };
    TimeNs now = Scheduler::get_instance().get_current_time();
    packet.generated_time = now;

    packet.ecn_capable_transport = true;
    packet.congestion_experienced = false;
    return packet;
}

void RdmaConnection::process_data_packet(const Packet& packet) {
    if (packet.packet_num < m_next_expected_packet_num) {
        LOG_ERROR(
            fmt::format("RDMA receiver got data packet {} with number smaller "
                        "than expected {}; ignored",
                        packet.to_string(), m_next_expected_packet_num));
    } else if (packet.packet_num == m_next_expected_packet_num) {
        process_expected_data_packet();
        while (!m_reorder_buffer.empty() &&
               m_reorder_buffer.front().has_value()) {
            m_reorder_buffer.pop_front();
            process_expected_data_packet();
        }
    } else {
        uint32_t diff = packet.packet_num - m_next_expected_packet_num;
        if (diff >= m_max_reorder_buffer_size) {
            LOG_ERROR(fmt::format(
                "RDMA receiver got data packet {} with number greater "
                "than expected {}; could not put it to reorder buffer; ignored",
                packet.to_string(), m_next_expected_packet_num));
            // TODO: send NAK
            return;
        }
        while (m_reorder_buffer.size() <= diff) {
            m_reorder_buffer.emplace_back(std::nullopt);
        }
        LOG_INFO(
            fmt::format("RDMA receiver got data packet {} with number greater "
                        "than expected {}; put it to reorder buffer;",
                        m_id, packet));
        m_reorder_buffer[diff].emplace(packet);
    }
}

void RdmaConnection::process_expected_data_packet() {
    m_next_expected_packet_num++;
    if (++m_data_packets_on_receiver >= m_ack_threshold) {
        m_data_packets_on_receiver = 0;
        send_ack();
    }
}

void RdmaConnection::send_ack() {
    Packet ack;
    if (m_next_expected_packet_num == 0) {
        LOG_ERROR(
            fmt::format("RDMA connection {}: cold not send ack: next expected "
                        "packet num = 0",
                        m_id));
        return;
    }
    ack.packet_num = m_next_expected_packet_num - 1;

    ack.sender_id = m_receiver->get_id();
    ack.sender_port = m_receiver_port;
    ack.receiver_port = m_sender_port;
    ack.receiver_id = m_sender->get_id();
    ack.size = M_ACK_SIZE;

    RdmaConnectionPtr conn = shared_from_this();

    ack.callback = [conn](const Packet& delivered_packet) {
        conn->process_ack(delivered_packet);
    };
    TimeNs now = Scheduler::get_instance().get_current_time();
    ack.generated_time = now;
    ack.sent_time = now;

    ack.ecn_capable_transport = true;
    ack.congestion_experienced = false;

    m_receiver->enqueue_packet(ack);
}

void RdmaConnection::process_ack(const Packet& ack) {
    PacketNum ack_num = ack.packet_num;
    if (ack_num < m_last_acked_pcn) {
        LOG_ERROR(
            fmt::format("RDMA connection {}: sender got ack with number "
                        "smaller than last acked PCN",
                        m_id));
        return;
    }
    while (m_last_acked_pcn < ack_num) {
        m_last_acked_pcn++;
        confirm_first_unconfirmed_packet();
    }
}

void RdmaConnection::confirm_first_unconfirmed_packet() {
    if (m_send_queue.empty()) {
        LOG_ERROR(
            fmt::format("RDMA connection {}: could not confirm packet with "
                        "empty send queue",
                        m_id));
        return;
    }
    const Packet& confirmed = m_send_queue.front();
    utils::Defer defer([this]() {
        m_send_queue.pop_front();
        if (m_send_queue.empty()) {
            m_dcqcn.stop();
        }
    });

    const DataId& data_id = confirmed.data_id;
    auto it = m_data_context_table.find(data_id);
    if (it == m_data_context_table.end()) {
        LOG_ERROR(
            fmt::format("RDMA connection {}: could not find context that "
                        "connresponds to data {}",
                        m_id, data_id.to_string()));
        return;
    }
    DataContext& data_ctx = it->second;
    data_ctx.delivered += m_packet_size;
    if (data_ctx.delivered >= data_ctx.total_size) {
        LOG_INFO(
            fmt::format("RDMA connection {} delivered data {}; call callback",
                        m_id, data_id.to_string()));
        data_ctx.callback();
    }
}

void RdmaConnection::send_ack_request() {
    Packet ack_request;
    ack_request.packet_num = m_next_packet_num++;

    ack_request.sender_id = m_sender->get_id();
    ack_request.sender_port = m_sender_port;
    ack_request.receiver_port = m_receiver_port;
    ack_request.receiver_id = m_receiver->get_id();
    ack_request.size = ACK_REQUEST_SIZE;

    RdmaConnectionPtr conn = shared_from_this();

    ack_request.callback =
        [conn]([[maybe_unused]] const Packet& delivered_packet) {
            conn->process_ack_request();
        };
    TimeNs now = Scheduler::get_instance().get_current_time();
    ack_request.generated_time = now;
    ack_request.sent_time = now;

    ack_request.ecn_capable_transport = true;
    ack_request.congestion_experienced = false;
    m_sender->enqueue_packet(ack_request);
}

void RdmaConnection::process_ack_request() { send_ack(); }

}  // namespace sim
