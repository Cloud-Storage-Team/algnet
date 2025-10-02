#include "connection/flow/i_flow.hpp"
#include "packet.hpp"

namespace sim {

inline Id get_flow_id(IFlow* flow_ptr) {
    return (flow_ptr == nullptr ? "" : flow_ptr->get_id());
}

}