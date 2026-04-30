#include "rdma_connection_parser.hpp"

#include <random>

#include "dcqcn_parser.hpp"

namespace sim {

static EndpointPorts generate_ports() {
    static constexpr size_t RANDOM_SEED = 31;
    static std::mt19937 rnd(RANDOM_SEED);

    static constexpr size_t MAX_PORT = 5000;

    static std::uniform_int_distribution<int> range(0, MAX_PORT);
    return EndpointPorts(range(rnd), range(rnd));
}

RdmaConnectionPtr parse_rdma_connection(std::shared_ptr<IHost> sender,
                                        std::shared_ptr<IHost> receiver,
                                        const ConfigNodeWithPreset& params,
                                        const Id& connection_id) {
    static const ConfigNodeWithPreset& dcqcn_node =
        params["dcqcn"].value_or_throw();

    DCQCN dcqcn = parse_dcqcn(dcqcn_node);

    FlowFourTuple ft(Endpoints(sender, receiver), generate_ports());

    return RdmaConnection::create_shared(connection_id, dcqcn, ft);
}

}  // namespace sim