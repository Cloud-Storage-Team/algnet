#pragma once
#include "connection/i_new_connection.hpp"
#include "event.hpp"

namespace sim {

/**
 * Adding data to the connection level at a specified time
 */
class AddDataToNewConnection : public Event {
public:
    AddDataToNewConnection(TimeNs when, std::shared_ptr<INewConnection> conn,
                        Data a_data, OnDeliveryCallback a_callback);
    ~AddDataToNewConnection() = default;
    void operator()() final;

private:
    std::shared_ptr<INewConnection> m_connection;
    Data m_data;
    OnDeliveryCallback m_callback;
};

}  // namespace sim
