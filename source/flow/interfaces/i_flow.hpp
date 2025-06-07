#pragma once

#include "device/interfaces/i_sender.hpp"
#include "device/interfaces/i_receiver.hpp"
#include "utils/identifier_factory.hpp"

namespace sim {

class IFlow : public Identifiable {
public:
    virtual void start() = 0;
    // Adds new packet to sending queue
    // This packet will be send at some time in future (depends on concrete
    // flow) Used in event Generate
    virtual Time create_new_data_packet() = 0;

    // Update the internal state according to some congestion control algorithm
    // Calls when data available for sending on corresponding device
    virtual void update(Packet packet, DeviceType type) = 0;
    virtual std::shared_ptr<ISender> get_sender() const = 0;
    virtual std::shared_ptr<IReceiver> get_receiver() const = 0;
};

}  // namespace sim
