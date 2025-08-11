#pragma once
#include <spdlog/fmt/fmt.h>
#include <yaml-cpp/yaml.h>

#include <memory>

#include "flow/tcp/tcp_flow.hpp"


namespace sim {

class FlowParser {
public:
    static std::shared_ptr<IFlow> parse_i_flow(const YAML::Node& key_node,
                                               const YAML::Node& value_node,
                                               Id sender_id, Id receiver_id);

private:
    class TcpCCParser {
    public:
        static std::unique_ptr<ITcpCC> parse_i_tcp_cc(const YAML::Node& cc_node,
                                                      Id flow_id);
    };

    static std::shared_ptr<TcpFlow> parse_tcp_flow(const YAML::Node& key_node,
                                                   const YAML::Node& value_node,
                                                   Id sender_id,
                                                   Id receiver_id);
};

}  // namespace sim
