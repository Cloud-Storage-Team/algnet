#pragma once
#include <yaml-cpp/yaml.h>

#include "device/receiver.hpp"
#include "parse_primitives.hpp"
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
Id parse_object<Sender>(const YAML::Node& key_node,
                        const YAML::Node& value_node) {
    (void)value_node;
    Id id = key_node.as<Id>();
    parse_object_helper<Sender>(id);
    return id;
}

template <>
Id parse_object<Receiver>(const YAML::Node& key_node,
                          const YAML::Node& value_node) {
    (void)value_node;
    Id id = key_node.as<Id>();
    parse_object_helper<Receiver>(id);
    return id;
}

template <>
Id parse_object<Switch>(const YAML::Node& key_node,
                        const YAML::Node& value_node) {
    (void)value_node;
    Id id = key_node.as<Id>();
    parse_object_helper<Switch>(id);
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

template <>
Id parse_object<Flow>(const YAML::Node& key_node,
                      const YAML::Node& value_node) {
    Id id = key_node.as<Id>();
    Id sender_id = value_node["sender_id"].as<Id>();
    Id receiver_id = value_node["receiver_id"].as<Id>();
    Size packet_size = value_node["packet_size"].as<Size>();
    Time packet_interval = value_node["packet_interval"].as<Time>();
    std::uint32_t number_of_packets =
        value_node["number_of_packets"].as<std::uint32_t>();

    std::shared_ptr<ISender> sender_ptr =
        IdentifierFactory::get_instance().get_object<ISender>(sender_id);
    std::shared_ptr<IReceiver> receiver_ptr =
        IdentifierFactory::get_instance().get_object<IReceiver>(receiver_id);

    parse_object_helper<Flow>(id, sender_ptr, receiver_ptr, packet_size,
                              packet_interval, number_of_packets);
    return id;
}

template <>
Id parse_object<TcpFlow>(const YAML::Node& key_node,
                         const YAML::Node& value_node) {
    Id id = key_node.as<Id>();
    Id sender_id = value_node["sender_id"].as<Id>();
    Id receiver_id = value_node["receiver_id"].as<Id>();
    Size packet_size = value_node["packet_size"].as<Size>();
    Time packet_interval = value_node["packet_interval"].as<Time>();
    std::uint32_t number_of_packets =
        value_node["number_of_packets"].as<std::uint32_t>();

    std::shared_ptr<ISender> sender_ptr =
        IdentifierFactory::get_instance().get_object<ISender>(sender_id);
    std::shared_ptr<IReceiver> receiver_ptr =
        IdentifierFactory::get_instance().get_object<IReceiver>(receiver_id);

    parse_object_helper<TcpFlow>(id, sender_ptr, receiver_ptr, packet_size,
                                 packet_interval, number_of_packets);
    return id;
}

}  // namespace sim
