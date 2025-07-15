#pragma once
#include <yaml-cpp/yaml.h>

namespace sim {

template <typename TTcpCC>
TTcpCC parse_tcp_cc(const YAML::Node& key_node, const YAML::Node& value_node);

}