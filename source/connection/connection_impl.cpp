#include "connection/connection_impl.hpp"

#include "logger/logger.hpp"
#include "scheduler.hpp"

namespace sim {

ConnectionImpl::ConnectionImpl(Id a_id, Id a_sender_id, Id a_receiver_id,
                               std::shared_ptr<IMPLB> mplb,
                               std::uint64_t a_num_packets_to_send)
    : m_id(a_id),
      m_sender_id(a_sender_id),
      m_receiver_id(a_receiver_id),
      m_mplb(std::move(mplb)),
      m_packets_to_send(a_num_packets_to_send) {}

Id ConnectionImpl::get_id() const { return m_id; }

void ConnectionImpl::start() { send_packets(); }

void ConnectionImpl::add_flow(std::shared_ptr<IFlow> flow) {
    m_flows.insert(flow);
    m_mplb->add_flow(flow);
}

void ConnectionImpl::delete_flow(std::shared_ptr<IFlow> flow) {
    m_flows.erase(flow);
    m_mplb->remove_flow(flow);
}

void ConnectionImpl::add_packets_to_send(std::uint64_t count_packets) {
    m_packets_to_send += count_packets;
}

void ConnectionImpl::update(const std::shared_ptr<IFlow>& flow,
                            const FlowSample sample) {
    if (!flow) {
        LOG_ERROR(fmt::format("Null flow in ConnectionImpl {} update; ignored",
                              m_id));
        return;
    }
    // Notify MPLB about received packet for metric updates
    m_mplb->notify_packet_confirmed(flow, sample);
    // Trigger next possible sending attempt
    send_packets();
}

bool ConnectionImpl::has_flows() const { return !m_flows.empty(); }

void ConnectionImpl::clear_flows() { m_flows.clear(); }

void ConnectionImpl::send_packets() {
    while (m_packets_to_send > 0) {
        auto flow = m_mplb->select_flow();
        if (!flow) {
            LOG_INFO(fmt::format(
                "No flow can send packets at the moment in connection {}",
                m_id));
            break;
        }
        if (!flow->can_send()) {
            continue;
        }
        flow->send_packet();
        --m_packets_to_send;
    }
}

}  // namespace sim
