#pragma once
#include "event.hpp"
#include "network/connection/i_new_connection.hpp"

namespace sim {

/**
 * Adding data to the connection level at a specified time
 */
class AddDataToNewConnection : public Event {
public:
    AddDataToNewConnection(TimeNs when, std::shared_ptr<IConnection> conn,
                           Data a_data, OnDeliveryCallback a_callback);
    ~AddDataToNewConnection() = default;
    void operator()() final;

private:
    std::shared_ptr<IConnection> m_connection;
    Data m_data;
    OnDeliveryCallback m_callback;
};

}  // namespace sim
