#include "connection/connection.hpp"

#include "logger/logger.hpp"
#include "scheduler.hpp"

namespace sim {

Connection::Connection(Id a_id, Id a_sender_id, Id a_receiver_id,
                       SizeByte a_packet_size, std::shared_ptr<IMPLB> mplb,
                       std::uint64_t a_num_packets_to_send)
    : m_id(a_id),
      m_sender_id(a_sender_id),
      m_receiver_id(a_receiver_id),
      m_packet_size(a_packet_size),
      m_mplb(std::move(mplb)),
      m_packets_to_send(a_num_packets_to_send) {}

Id Connection::get_id() const { return m_id; }

void Connection::start() { send_packets(); }

void Connection::add_flow(std::shared_ptr<IFlow> flow) {
    m_flows.push_back(flow);
    m_mplb->add_flow(flow);
}

void Connection::delete_flow(std::shared_ptr<IFlow> flow) {
    m_flows.erase(std::remove(m_flows.begin(), m_flows.end(), flow),
                  m_flows.end());
    m_mplb->remove_flow(flow);
}

void Connection::add_packets_to_send(std::uint64_t packets_num) {
    m_packets_to_send += packets_num;
}

void Connection::update(std::shared_ptr<IFlow> flow, const FlowSample sample) {
    if (!flow) {
        LOG_ERROR("Null flow in update()");
        return;
    }
    // Notify MPLB about received packet for metric updates
    m_mplb->notify_ack_received(flow, sample);

    // Trigger next possible sending attempt
    send_packets();
}

void Connection::send_packets() {
    while (m_packets_to_send > 0) {
        auto flow = m_mplb->select_flow();
        if (!flow) {
            LOG_WARN("No flow can send packets at the moment");
            break;
        }

        if (!flow->can_send()) {
            continue;
        }

        flow->send_packet();
        m_packets_to_send--;

        // m_mplb->notify_packet_sent(flow, packet);
    }
}

}  // namespace sim
