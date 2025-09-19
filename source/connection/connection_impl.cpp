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
    m_mplb->add_flow(flow);
}

void ConnectionImpl::delete_flow(std::shared_ptr<IFlow> flow) {
    m_flows.erase(flow);
    m_mplb->remove_flow(flow);
}

void ConnectionImpl::add_data_to_send(SizeByte data) { m_data_to_send += data; }

void ConnectionImpl::update(const std::shared_ptr<IFlow>& flow) {
    if (!flow) {
        LOG_ERROR(fmt::format("Null flow in ConnectionImpl {} update; ignored",
                              m_id));
        return;
    }
    // Notify MPLB about received packet for metric updates
    m_mplb->notify_packet_confirmed(flow);
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
        SizeByte data = std::min(quota, m_data_to_send);

        // if (m_data_to_send < flow->get_packet_size()) {
        //     flow->send_packet();
        //     m_data_to_send = SizeByte(0);
        //     break;
        // } else if (quota == SizeByte(0)) {
        //     throw std::runtime_error(fmt::format(
        //         "MPLB returned flow {} with zero quota in connection {}",
        //         flow->get_id(), m_id));
        // } else if (quota < flow->get_packet_size()) {
        //     // Waiting for the quota to be at least the size of the package
        //     break;
        // }

        flow->send_data(data);
        m_data_to_send -= data;
    }
    if (m_data_to_send.value() == 1) {
        std::cout << "11111111";
    }
    std::cout << "Connection " << m_id << " has " << m_data_to_send.value()
              << " bytes left to send\n";
}

std::shared_ptr<IHost> ConnectionImpl::get_sender() const {
    return m_src.lock();
}

std::shared_ptr<IHost> ConnectionImpl::get_receiver() const {
    return m_dest.lock();
}

}  // namespace sim
