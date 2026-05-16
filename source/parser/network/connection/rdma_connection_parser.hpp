#pragma once
#include "network/connection/rdma/rdma_connectrion.hpp"
#include "parser/config_reader/config_node_with_preset.hpp"

namespace sim {

RdmaConnectionPtr parse_rdma_connection(std::shared_ptr<IHost> sender,
                                        std::shared_ptr<IHost> receiver,
                                        const ConfigNodeWithPreset& params,
                                        const Id& connection_id);

}