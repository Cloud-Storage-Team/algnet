#include "add_data_to_connection.hpp"

namespace sim {

AddDataToConnection::AddDataToConnection(TimeNs a_when,
                                         std::shared_ptr<IConnection> a_conn,
                                         Data a_data,
                                         OnDeliveryCallback a_callback)
    : Event(a_when),
      m_connection(a_conn),
      m_data(a_data),
      m_callback(a_callback) {}

void AddDataToConnection::operator()() {
    auto res = m_connection->send_data(m_data, m_callback);
    if (!res.has_value()) {
        LOG_ERROR(std::move(res.error()));
    }
}

}  // namespace sim
