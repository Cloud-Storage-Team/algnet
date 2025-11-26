#include "add_data_to_connection.hpp"

namespace sim {

AddDataToConnection::AddDataToConnection(TimeNs a_when,
                                         std::shared_ptr<IConnection> a_conn,
                                         SizeByte a_size,
                                         OnDeliveryCallback a_callback)
    : Event(a_when),
      m_connection(a_conn),
      m_size(a_size),
      m_callback(a_callback) {}

void AddDataToConnection::operator()() {
    // TODO: eliminate counter
    static int counter = 0;
    counter++;
    if (m_connection.expired()) {
        LOG_ERROR("Connection expired; can't add data to it");
        return;
    }

    m_connection.lock()->add_data_to_send(
        Data(fmt::format("send_data_{}", counter), m_size), m_callback);
}

}  // namespace sim
