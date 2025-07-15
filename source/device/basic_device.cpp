#include "device/basic_device.hpp"

namespace sim {

    BasicDevice::BasicDevice(Id a_id, std::unique_ptr<IHasher> a_hasher): 
        RoutingModule(a_id, std::move(a_hasher)) {};

    bool BasicDevice::notify_about_arrival(Time arrival_time) {
        (void)arrival_time;
        return false;
    };

}  // namespace sim