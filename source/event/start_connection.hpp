#pragma once

#include "event/event.hpp"
#include "connection/connection.hpp"

namespace sim {

class StartConnection final : public Event {
public:
    StartConnection(TimeNs time, std::weak_ptr<Connection> c)
        : Event(time), m_conn(std::move(c)) {}

    void operator()() override {
        if (auto conn = m_conn.lock()) {
            conn->kick();
        }
    }

private:
    std::weak_ptr<Connection> m_conn;
};

} // namespace sim
