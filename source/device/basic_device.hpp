#pragma once

#include <unordered_map>

#include "device/routing_module.hpp"
#include "device/interfaces/i_device.hpp"

namespace sim {
    
class BasicDevice : public virtual IDevice, public RoutingModule {
public:
    BasicDevice(Id a_id = "", std::unique_ptr<IHasher> a_hasher = nullptr);
    virtual ~BasicDevice() = default;

    virtual bool notify_about_arrival(Time arrival_time);
};

}  // namespace sim
