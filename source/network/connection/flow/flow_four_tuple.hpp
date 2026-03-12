#pragma once
#include "endpoint_ports.hpp"
#include "endpoints.hpp"

namespace sim {

struct FlowFourTuple : Endpoints, EndpointPorts {
    FlowFourTuple(Endpoints endpoints, EndpointPorts ports = {})
        : Endpoints(endpoints), EndpointPorts(ports) {}
};

}  // namespace sim
