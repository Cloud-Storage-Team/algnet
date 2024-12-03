#include "Flow.hpp"

Flow::Flow(const NetworkDevice& src, const NetworkDevice& dst):
    source_node(std::make_shared<NetworkDevice>(src)),
    destination_node(std::make_shared<NetworkDevice>(dst)) { }