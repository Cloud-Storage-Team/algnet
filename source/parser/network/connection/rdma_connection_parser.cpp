#include "rdma_connection_parser.hpp"

#include <random>

#include "dcqcn_parser.hpp"
#include "parser/parse_utils.hpp"

namespace sim {

static EndpointPorts generate_ports() {
    // static constexpr size_t RANDOM_SEED = 42;
    // static std::mt19937 rnd(RANDOM_SEED);

    // static constexpr size_t MAX_PORT = 5000;

    // static std::uniform_int_distribution<int> range(0, MAX_PORT);
    // return EndpointPorts(range(rnd), range(rnd));
    return EndpointPorts{0, 1};
}

RdmaConnectionPtr parse_rdma_connection(std::shared_ptr<IHost> sender,
                                        std::shared_ptr<IHost> receiver,
                                        const ConfigNodeWithPreset& params,
                                        const Id& connection_id) {
    static const ConfigNodeWithPreset& dcqcn_node =
        params["dcqcn"].value_or_throw();

    DCQCN dcqcn = parse_dcqcn(dcqcn_node);

    EndpointPorts ports;
    if (auto exp_ports_node = params["ports"]) {
        auto ports_node = exp_ports_node.value();
        ports.sender_port = ports_node["sender"].as<Port>().value_or_throw();
        ports.receiver_port = ports_node["receiver"].as<Port>().value_or_throw();
    } else {
        ports = generate_ports();
    }

    FlowFourTuple ft(Endpoints(sender, receiver), ports);

    RdmaParams rdma_params{connection_id, dcqcn, ft};

    return RdmaConnection::create_shared(rdma_params);
}

}  // namespace sim
