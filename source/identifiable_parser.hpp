#pragma once
#include <yaml-cpp/yaml.h>

#include "device/receiver.hpp"
#include "utils/identifier_factory.hpp"

namespace sim {

template <typename T>
Id parse_object(const YAML::Node& config);

template <>
Id parse_object<Receiver>(const YAML::Node& config) {}

}  // namespace sim