#pragma once
#include <spdlog/fmt/fmt.h>
#include <yaml-cpp/yaml.h>

#include <memory>

#include "flow/new_flow.hpp"
#include "parser/parse_primitives.hpp"
#include "utils/identifier_factory.hpp"

namespace sim {

// parses objects from yaml, adds them to IdentifierFactory and returns id.
// Typical object scheme: name:
//  field_1 : value_1
//  field_2 : value_2
template <typename T>
Id parse_object(const YAML::Node& key_node, const YAML::Node& value_node);

template <typename T>
std::string concatenate(T arg) {
    std::ostringstream oss;
    oss << arg;
    return oss.str();
}

template <typename T, typename... Args>
static std::string concatenate(T first, Args... args) {
    std::ostringstream oss;
    oss << first << ", ";
    oss << concatenate(args...);
    return oss.str();
}

template <typename T, typename... Args>
static void parse_object_helper(Args... args) {
    if (!IdentifierFactory::get_instance().add_object<T>(args...)) {
        throw std::runtime_error(fmt::format(
            "Can not add object {}({}): object with this id already exists",
            typeid(T).name(), concatenate(args...)));
    }
}

template <>
Id parse_object<Switch>(const YAML::Node& key_node,
                        const YAML::Node& value_node) {
    (void)value_node;
    Id id = key_node.as<Id>();
    if (value_node["threshold"]) {
        double threshold = value_node["threshold"].as<double>();
        bool is_valid = (threshold > 0.0 && threshold <= 1.0);
        parse_object_helper<Switch>(id, is_valid, threshold);
    } else {
        parse_object_helper<Switch>(id);
    }
    return id;
}

template <>
Id parse_object<Host>(const YAML::Node& key_node,
                      const YAML::Node& value_node) {
    (void)value_node;
    Id id = key_node.as<Id>();
    parse_object_helper<Host>(id);
    return id;
}

template <>
Id parse_object<Link>(const YAML::Node& key_node,
                      const YAML::Node& value_node) {
    Id id = key_node.as<Id>();
    Id from_id = value_node["from"].as<Id>();
    Id to_id = value_node["to"].as<Id>();
    auto from_ptr =
        IdentifierFactory::get_instance().get_object<IRoutingDevice>(from_id);
    auto to_ptr =
        IdentifierFactory::get_instance().get_object<IRoutingDevice>(to_id);

    if (from_ptr == nullptr) {
        LOG_ERROR("Failed to find link's source");
        return "";
    }

    if (to_ptr == nullptr) {
        LOG_ERROR("Failed to find link's destination");
        return "";
    }

    uint32_t latency = 0;
    if (value_node["latency"]) {
        latency = parse_latency(value_node["latency"].as<std::string>());
    } else {
        LOG_WARN(fmt::format(
            "latency does not set for link {}; use default value {}", id,
            latency));
    }

    uint32_t speed = 1;
    if (value_node["throughput"]) {
        speed = parse_throughput(value_node["throughput"].as<std::string>());
    } else {
        LOG_WARN(fmt::format(
            "speed does not set for link {}; use default value {}", id, speed));
    }

    uint32_t ingress_buffer_size = 4096;
    if (value_node["ingress_buffer_size"]) {
        ingress_buffer_size = parse_buffer_size(
            value_node["ingress_buffer_size"].as<std::string>());
    } else {
        LOG_WARN(
            fmt::format("ingress buffer size does not set for link {}; use "
                        "default value {}",
                        id, ingress_buffer_size));
    }

    uint32_t egress_buffer_size = 4096;
    if (value_node["egress_buffer_size"]) {
        egress_buffer_size = parse_buffer_size(
            value_node["egress_buffer_size"].as<std::string>());
    } else {
        LOG_WARN(fmt::format(
            "egress buffer size does not set for link {}; use default value {}",
            id, egress_buffer_size));
    }

    parse_object_helper<Link>(id, from_ptr, to_ptr, speed, latency,
                              egress_buffer_size, ingress_buffer_size);

    return id;
}

struct FlowCommon {
    Id id;
    std::shared_ptr<IHost> sender_ptr;
    std::shared_ptr<IHost> receiver_ptr;
    Size packet_size;
    Time packet_interval;
    std::uint32_t number_of_packets;
};

static FlowCommon parse_flow_common(const YAML::Node& key_node,
                                    const YAML::Node& value_node) {
    FlowCommon result;
    result.id = key_node.as<Id>();

    Id sender_id = value_node["sender_id"].as<Id>();
    result.sender_ptr =
        IdentifierFactory::get_instance().get_object<IHost>(sender_id);

    Id receiver_id = value_node["receiver_id"].as<Id>();
    result.receiver_ptr =
        IdentifierFactory::get_instance().get_object<IHost>(receiver_id);

    result.packet_size = value_node["packet_size"].as<Size>();
    result.number_of_packets =
        value_node["number_of_packets"].as<std::uint32_t>();
    result.packet_interval = value_node["packet_interval"].as<Time>();

    return result;
}

template <>
Id parse_object<NewFlow>(const YAML::Node& key_node,
                         const YAML::Node& value_node) {
    FlowCommon flow_common = parse_flow_common(key_node, value_node);

    parse_object_helper<NewFlow>(
        flow_common.id, dynamic_pointer_cast<IHost>(flow_common.sender_ptr),
        dynamic_pointer_cast<IHost>(flow_common.receiver_ptr),
        flow_common.packet_size, flow_common.packet_interval,
        flow_common.number_of_packets);
    return flow_common.id;
}

template <>
Id parse_object<NewTcpFlow>(const YAML::Node& key_node,
                            const YAML::Node& value_node) {
    FlowCommon flow_common = parse_flow_common(key_node, value_node);

    parse_object_helper<NewTcpFlow>(
        flow_common.id, dynamic_pointer_cast<IHost>(flow_common.sender_ptr),
        dynamic_pointer_cast<IHost>(flow_common.receiver_ptr),
        flow_common.packet_size, flow_common.packet_interval,
        flow_common.number_of_packets);
    return flow_common.id;
}

}  // namespace sim
