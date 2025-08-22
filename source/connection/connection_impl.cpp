#include "connection/connection_impl.hpp"

#include "logger/logger.hpp"
#include "scheduler.hpp"

namespace sim {

ConnectionImpl::ConnectionImpl(Id a_id, std::shared_ptr<IHost> a_src,
                               std::shared_ptr<IHost> a_dest,
                               std::shared_ptr<IMPLB> a_mplb,
                               std::uint64_t a_num_packets_to_send)
    : m_id(a_id),
      m_src(a_src),
      m_dest(a_dest),
      m_mplb(std::move(a_mplb)),
      m_packets_to_send(a_num_packets_to_send) {}

Id ConnectionImpl::get_id() const { return m_id; }

void ConnectionImpl::start() { send_packets(); }

void ConnectionImpl::add_flow(std::shared_ptr<IFlow> flow) {
    m_flows.insert(flow);
    FlowSample init_sample{.ack_recv_time = TimeNs(0),
                          .packet_sent_time = TimeNs(0),
                          .packets_in_flight = 0,
                          .delivery_rate = SpeedGbps(0),
                          .send_quota = flow->get_sending_quota()};

    m_mplb->add_flow(flow, init_sample);
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

std::set<std::shared_ptr<IFlow>> ConnectionImpl::get_flows() const {
    return m_flows;
}

void ConnectionImpl::clear_flows() {
    m_flows.clear();
    m_mplb->clear_flows();
}

void ConnectionImpl::send_packets() {
    while (m_packets_to_send > 0) {
        auto flow = m_mplb->select_flow();
        if (!flow) {
            LOG_INFO(fmt::format(
                "No flow can send packets at the moment in connection {}",
                m_id));
            break;
        }
        if (!flow->get_sending_quota()) {
            throw std::runtime_error(fmt::format(
                "MPLB returned flow {} with zero quota in connection {}",
                flow->get_id(), m_id));
        }
        flow->send_packet();
        --m_packets_to_send;
    }
}

std::shared_ptr<IHost> ConnectionImpl::get_sender() const {
    return m_src.lock();
}

std::shared_ptr<IHost> ConnectionImpl::get_receiver() const {
    return m_dest.lock();
}

}  // namespace sim
