#include "connection/connection_impl.hpp"

#include "logger/logger.hpp"
#include "scheduler.hpp"

namespace sim {

ConnectionImpl::ConnectionImpl(Id a_id, std::shared_ptr<IHost> a_src,
                               std::shared_ptr<IHost> a_dest,
                               std::shared_ptr<IMPLB> a_mplb,
                               SizeByte a_data_to_send)
    : m_id(a_id),
      m_src(a_src),
      m_dest(a_dest),
      m_mplb(std::move(a_mplb)),
      m_data_to_send(a_data_to_send) {}

Id ConnectionImpl::get_id() const { return m_id; }

void ConnectionImpl::start() { send_data(); }

void ConnectionImpl::add_flow(std::shared_ptr<IFlow> flow) {
    m_flows.insert(flow);
    FlowSample init_sample{.rtt = TimeNs(0),
                           .inflight = SizeByte(0),
                           .delivery_rate = SpeedGbps(0),
                           .send_quota = flow->get_sending_quota()};

    m_mplb->add_flow(flow, init_sample);
}

void ConnectionImpl::delete_flow(std::shared_ptr<IFlow> flow) {
    m_flows.erase(flow);
    m_mplb->remove_flow(flow);
}

void ConnectionImpl::add_data_to_send(SizeByte data) {
    m_data_to_send += data;
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
    send_data();
}

std::set<std::shared_ptr<IFlow>> ConnectionImpl::get_flows() const {
    return m_flows;
}

void ConnectionImpl::clear_flows() {
    m_flows.clear();
    m_mplb->clear_flows();
}

void ConnectionImpl::send_data() {
    while (m_data_to_send > SizeByte(0)) {
        auto flow = m_mplb->select_flow();
        if (!flow) {
            LOG_INFO(fmt::format(
                "No flow can send packets at the moment in connection {}",
                m_id));
            break;
        }
        SizeByte quota = flow->get_sending_quota();
        if (quota == SizeByte(0)) {
            throw std::runtime_error(fmt::format(
                "MPLB returned flow {} with zero quota in connection {}",
                flow->get_id(), m_id));
        }
        flow->send_data(quota);
        m_data_to_send -= quota;
        
    }
}

std::shared_ptr<IHost> ConnectionImpl::get_sender() const {
    return m_src.lock();
}

std::shared_ptr<IHost> ConnectionImpl::get_receiver() const {
    return m_dest.lock();
}

}  // namespace sim
