#pragma once
#include "dcqcn/dcqcn.hpp"
#include "network/connection/flow/flow_four_tuple.hpp"

namespace sim {

struct RdmaParams {
    Id id;
    DCQCN dcqcn;
    FlowFourTuple ft;
    SizeByte packet_size = SizeByte(1500ul);
    uint32_t send_window = 256;
    uint32_t ack_threshold = 32;
    SizeByte reorder_buffer_size = Size<KByte>(64ul);
};
}  // namespace sim
