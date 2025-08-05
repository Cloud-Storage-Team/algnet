#pragma once
#include <spdlog/fmt/fmt.h>
#include <yaml-cpp/yaml.h>

#include <memory>

#include "link/link.hpp"

namespace sim {

class ParseLink {
public:
    static std::shared_ptr<ILink> parse_i_link(const YAML::Node& key_node,
                                                 const YAML::Node& value_node);
private:
    static std::shared_ptr<Link> parse_default_link(const YAML::Node& key_node,
                                                 const YAML::Node& value_node);
};

}  // namespace sim
