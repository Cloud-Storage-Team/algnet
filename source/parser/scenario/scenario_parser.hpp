#pragma once
#include <filesystem>

#include "scenario/scenario.hpp"

namespace sim {

Scenario parse_scenario(const std::filesystem::path& path);

}
