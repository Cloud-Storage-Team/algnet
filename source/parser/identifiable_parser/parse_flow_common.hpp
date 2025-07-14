#pragma once

#include <yaml-cpp/yaml.h>

#include "device/interfaces/i_host.hpp"

namespace sim {

struct FlowCommon {
    Id id;
    std::shared_ptr<IHost> sender_ptr;
    std::shared_ptr<IHost> receiver_ptr;
    Size packet_size;
    Time packet_interval;
    std::uint32_t number_of_packets;
};

FlowCommon parse_flow_common(const YAML::Node& key_node,
                             const YAML::Node& value_node);

}  // namespace sim