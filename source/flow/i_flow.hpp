#pragma once

#include "device/interfaces/i_host.hpp"
#include "utils/identifier_factory.hpp"
#include "utils/statistics.hpp"

namespace sim {

class IConnection;

class IFlow : public Identifiable {
public:
    // Update the internal state according to some congestion control algorithm
    // Calls when data available for sending on corresponding device
    virtual void update(Packet packet) = 0;
    virtual SizeByte get_sending_quota() const = 0;
    virtual TimeNs get_last_rtt() const = 0;
    virtual void send_data(SizeByte data) = 0;
    virtual std::shared_ptr<IConnection> get_conn() const = 0;
    virtual SizeByte get_delivered_data_size() const = 0;
    virtual TimeNs get_fct() const = 0;
    virtual std::shared_ptr<IHost> get_sender() const = 0;
    virtual std::shared_ptr<IHost> get_receiver() const = 0;
};

}  // namespace sim
