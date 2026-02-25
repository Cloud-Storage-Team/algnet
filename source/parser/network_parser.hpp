#pragma once
#include <filesystem>

#include "simulator/network/network.hpp"

namespace sim {

Network parse_network(const std::filesystem::path &path);

}
