#pragma once

#include "device/interfaces/i_host.hpp"
#include "utils/identifier_factory.hpp"

namespace sim {

class IFlow : public Identifiable {
public:
    // Start sending packets
    virtual void start() = 0;

    // Update the internal state according to some congestion control algorithm
    // Calls when data available for sending on corresponding device
    virtual void update(Packet packet) = 0;

    virtual SizeByte get_delivered_data_size() const = 0;

    virtual std::shared_ptr<IHost> get_sender() const = 0;
    virtual std::shared_ptr<IHost> get_receiver() const = 0;
};

}  // namespace sim
