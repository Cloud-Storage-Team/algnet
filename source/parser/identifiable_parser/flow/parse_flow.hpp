#pragma once
#include <spdlog/fmt/fmt.h>
#include <yaml-cpp/yaml.h>

#include <memory>

#include "flow/tcp/tcp_flow.hpp"

namespace sim {

class ParseFlow {
public:
    static std::shared_ptr<IFlow> parse_i_flow(const YAML::Node& key_node,
                                                 const YAML::Node& value_node);
private:
    class ParseTcpCC {
    public:
        static std::unique_ptr<ITcpCC> parse_i_tcp_cc(const YAML::Node& key_node,
                                                        const YAML::Node& value_node);
    };

    static std::shared_ptr<TcpFlow> parse_tcp_flow(const YAML::Node& key_node,
                                                 const YAML::Node& value_node);
};

}  // namespace sim
