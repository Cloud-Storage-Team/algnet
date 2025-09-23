#include "add_data_to_connection.hpp"

namespace sim {

AddDataToConnection::AddDataToConnection(TimeNs a_when,
                                         std::shared_ptr<IConnection> a_conn,
                                         SizeByte a_amount)
    : Event(a_when), m_connection(a_conn), m_amount(a_amount) {}

void AddDataToConnection::operator()() {
    if (m_connection.expired()) {
        LOG_ERROR("Connection expired; can not start it");
        return;
    }
    m_connection.lock()->add_data_to_send(m_amount);
}

}  // namespace sim