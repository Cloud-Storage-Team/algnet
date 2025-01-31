#pragma once

#include "flow.hpp"
#include "node.hpp"

namespace sim {

struct Packet {
    int size;
    Node* src;
    Node* dest;
    Flow* flow;
};

}  // namespace sim
