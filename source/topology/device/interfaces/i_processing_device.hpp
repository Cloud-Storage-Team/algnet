#pragma once

#include "types.hpp"

namespace sim {

class IProcessingDevice {
public:
    virtual ~IProcessingDevice() = default;

    // One step of device work cycle;
    // e.g. see next inlink, take one packet from it,
    // and do smth with it (send further, send ask etc)
    virtual TimeNs process() = 0;
};

}  // namespace sim
