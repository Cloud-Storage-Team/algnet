#pragma once

#include "device.hpp"
#include "flow.hpp"

namespace sim {

struct Packet {
    int size;
    Device* src;
    Device* dest;
    Flow* flow;
};

}  // namespace sim
