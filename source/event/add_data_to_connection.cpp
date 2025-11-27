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
    if (m_connection.expired()) {
        LOG_ERROR("Connection expired; can't add data to it");
        return;
    }

    m_connection.lock()->add_data_to_send(m_data, m_callback);
}

}  // namespace sim
