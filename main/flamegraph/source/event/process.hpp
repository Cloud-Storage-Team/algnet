#pragma once
#include "device/interfaces/i_processing_device.hpp"
#include "event.hpp"

namespace sim {

/**
 * Dequeue a packet from the device ingress buffer
 * and start processing at the device.
 */
class Process : public Event {
public:
    Process(TimeNs a_time, std::weak_ptr<IProcessingDevice> a_device);
    ~Process() = default;
    void operator()() final;

private:
    std::weak_ptr<IProcessingDevice> m_device;
};

}  // namespace sim