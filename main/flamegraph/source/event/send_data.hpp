#pragma once
#include "device/interfaces/i_host.hpp"
#include "event.hpp"

namespace sim {

/**
 * Dequeue a packet from the device ingress buffer
 * and start processing at the device.
 */
class SendData : public Event {
public:
    SendData(TimeNs a_time, std::weak_ptr<IHost> a_device);
    ~SendData() = default;
    void operator()() final;

private:
    std::weak_ptr<IHost> m_device;
};

}  // namespace sim