#pragma once
#include <filesystem>

#include "network/network.hpp"

namespace sim {

Network parse_network(const std::filesystem::path &path);

}
