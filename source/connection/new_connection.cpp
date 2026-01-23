#include "new_connection.hpp"

namespace sim {

std::shared_ptr<NewConnection> NewConnection::create(
    Id a_id, std::shared_ptr<INewMPLB> a_mplb) {
    return std::shared_ptr<NewConnection>(
        new NewConnection(std::move(a_id), a_mplb));
}

NewConnection::NewConnection(Id a_id, std::shared_ptr<INewMPLB> a_mplb)
    : m_id(std::move(a_id)), m_context{SizeByte(0), SizeByte(0), a_mplb} {}

utils::StrExpected<void> NewConnection::send_data(Data data,
                                                  OnDeliveryCallback callback) {
    DataId data_id = data.id;
    if (m_data_context_table.contains(data_id)) {
        return std::unexpected(
            fmt::format("Connection {}: could not add data {}: data with given "
                        "id already sent",
                        m_id, data.to_string()));
    }
    m_data_context_table[data.id] =
        DataContext{data.size, SizeByte(0), SizeByte(0), callback};
    m_context.total_data_added += data.size;
    m_sending_queue.push(data_id);

    send_new_portion();
    return {};
}

ConnectionContext NewConnection::get_context() const { return m_context; }

Id NewConnection::get_id() const { return m_id; }

void NewConnection::send_new_portion() {
    while (!m_sending_queue.empty()) {
        DataId id = m_sending_queue.front();

        auto it = m_data_context_table.find(id);
        if (it == m_data_context_table.end()) {
            LOG_ERROR(
                fmt::format("Could not send new portion of data with id {}: no "
                            "context associated with it",
                            id.to_string()));
            m_sending_queue.pop();
            continue;
        }

        DataContext& context = it->second;
        if (context.sent >= context.total_size) {
            m_sending_queue.pop();
            continue;
        }

        SizeByte quota = m_context.mplb->get_context().sending_quota;
        if (quota == SizeByte(0)) {
            LOG_ERROR(fmt::format(
                "Sending quota is zero; could not send data with id {}",
                id.to_string()));
            break;
        }

        SizeByte delivery_size =
            std::min(context.total_size - context.sent, quota);

        std::weak_ptr<NewConnection> connection_weak = shared_from_this();

        OnDeliveryCallback callback = [connection_weak, id, delivery_size]() {
            if (connection_weak.expired()) {
                LOG_ERROR(
                    fmt::format("Pointer to connection expired; could not call "
                                "its callback on "
                                "delivery portion of data with id {}",
                                id.to_string()));
                return;
            }
            auto connection = connection_weak.lock();
            connection->m_context.total_data_confirmed += delivery_size;

            auto it = connection->m_data_context_table.find(id);
            if (it != connection->m_data_context_table.end()) {
                DataContext& context = it->second;
                context.delivered += delivery_size;
                if (context.delivered >= context.total_size) {
                    LOG_INFO(fmt::format(
                        "All data with id {} delivered; call callback",
                        id.to_string()));
                    context.callback();
                }
            } else {
                LOG_ERROR(fmt::format(
                    "Connection {}: could not find context for data with id {}",
                    connection->m_id, id.to_string()));
            }
            connection->send_new_portion();
        };

        context.sent += delivery_size;
        m_context.mplb->send_data(Data(id, delivery_size), callback)
            .log_err_if_not_present(
                fmt::format("Error on sending data in connection {}", m_id));
    }
}

}  // namespace sim
