#pragma once

#include "event/event.hpp"
#include "connection/connection.hpp"

namespace sim {

class ConnectionSend final : public Event {
public:
    ConnectionSend(TimeNs time, std::weak_ptr<Connection> c)
        : Event(time), m_conn(std::move(c)) {}

    void operator()() override {
        if (auto conn = m_conn.lock()) {
            conn->try_send_once();
        }
    }

private:
    std::weak_ptr<Connection> m_conn;
};

} // namespace sim
