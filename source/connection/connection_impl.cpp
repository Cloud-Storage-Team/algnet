#include "connection/connection_impl.hpp"

#include "event/add_data_to_connection.hpp"
#include "logger/logger.hpp"
#include "scheduler.hpp"

namespace sim {

ConnectionImpl::ConnectionImpl(Id a_id, std::shared_ptr<IHost> a_src,
                               std::shared_ptr<IHost> a_dest,
                               std::shared_ptr<IMPLB> a_mplb)
    : m_id(a_id),
      m_src(a_src),
      m_dest(a_dest),
      m_mplb(std::move(a_mplb)),
      m_total_data_added(0) {}

Id ConnectionImpl::get_id() const { return m_id; }

void ConnectionImpl::add_flow(std::shared_ptr<IFlow> flow) {
    m_flows.insert(flow);
    m_mplb->add_flow(flow);
}

void ConnectionImpl::delete_flow(std::shared_ptr<IFlow> flow) {
    m_flows.erase(flow);
    m_mplb->remove_flow(flow);
}

utils::StrExpected<void> ConnectionImpl::add_data_to_send(
    Data data, OnDeliveryCallback callback) {
    DataId data_id = data.id;
    if (m_data_context_table.contains(data_id)) {
        return std::unexpected(
            fmt::format("Connection {}: could not add data {}: data with given "
                        "id already sent",
                        m_id, data.to_string()));
    }
    m_sending_queue.emplace(data_id);
    m_data_context_table[data.id] =
        DataContext{data.size, SizeByte(0), SizeByte(0), callback};
    m_total_data_added += data.size;
    send_data();
    return {};
}

SizeByte ConnectionImpl::get_total_data_added() const {
    return m_total_data_added;
}

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
    while (!m_sending_queue.empty()) {
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
        DataId data_id = m_sending_queue.front();
        auto it = m_data_context_table.find(data_id);
        if (it == m_data_context_table.end()) {
            LOG_ERROR(
                fmt::format("Connection {}: could not find context for "
                            "data id {}; ignored",
                            m_id, data_id));
            return;
        }
        DataContext& context = it->second;
        SizeByte send_size = std::min(quota, context.total_size - context.sent);

        std::weak_ptr<ConnectionImpl> connection_weak = shared_from_this();
        std::function<void(DataId id, SizeByte size)> callback =
            [connection_weak](DataId data_id, SizeByte size) {
                if (connection_weak.expired()) {
                    LOG_ERROR(fmt::format("Could not "));
                }
                std::shared_ptr<ConnectionImpl> connection =
                    connection_weak.lock();
                Id id = connection->m_id;

                auto it = connection->m_data_context_table.find(data_id);
                if (it == connection->m_data_context_table.end()) {
                    LOG_ERROR(
                        fmt::format("Connection {}: could not find context for "
                                    "data id {}; ignored",
                                    id, data_id));
                    return;
                }
                DataContext& context = it->second;

                context.delivered += size;
                if (context.delivered == context.total_size) {
                    LOG_INFO(
                        fmt::format("Connection {} successefully deliveder "
                                    "data with id {} & size {}",
                                    connection->m_id, context.total_size));
                    context.callback();
                }
            };

        // TODO: pass callback to flow->send_data instead calling it here
        flow->send_data(send_size);
        context.sent += send_size;
        if (context.sent == context.total_size) {
            m_sending_queue.pop();
        }
        callback(data_id, send_size);
    }
}

std::shared_ptr<IHost> ConnectionImpl::get_sender() const {
    return m_src.lock();
}

std::shared_ptr<IHost> ConnectionImpl::get_receiver() const {
    return m_dest.lock();
}

}  // namespace sim
