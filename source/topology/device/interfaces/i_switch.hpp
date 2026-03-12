#pragma once

#include "topology/device/interfaces/i_device.hpp"

namespace sim {

class ISwitch : public virtual IDevice {
public:
    virtual ~ISwitch() = default;
};

}  // namespace sim
