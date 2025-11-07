#pragma once
#include "connection/i_connection.hpp"
#include "event.hpp"

namespace sim {

/**
 * Adding data to the connection level at a specified time
 */
class AddDataToConnection : public Event {
public:
    AddDataToConnection(TimeNs when, std::shared_ptr<IConnection> conn,
                        SizeByte size);
    ~AddDataToConnection() = default;
    void operator()() final;

private:
    std::weak_ptr<IConnection> m_connection;
    SizeByte m_size;
};

}  // namespace sim
