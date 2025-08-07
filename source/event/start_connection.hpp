#pragma once
#include "event.hpp"
#include "connection/connection.hpp"

namespace sim {

/**
 * Run connection at specified time
 */
class StartConnection : public Event {
public:
    StartConnection(TimeNs a_time, std::weak_ptr<Connection> connection);
    ~StartConnection() = default;
    void operator()() final;

private:
    std::weak_ptr<Connection> m_connection;
};

}