#include "start_connection.hpp"

namespace sim {

StartConnection::StartConnection(TimeNs a_time, std::weak_ptr<Connection> a_connection)
    : Event(a_time), m_connection(a_connection) {}

void StartConnection::operator()() {
    if (m_connection.expired()) {
        return;
    }

    m_connection.lock()->start();
}

}