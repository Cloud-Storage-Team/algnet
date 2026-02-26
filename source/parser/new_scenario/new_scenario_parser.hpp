#pragma once
#include <filesystem>

#include "new_scenario/new_scenario.hpp"

namespace sim {

NewScenario parse_scenario(const std::filesystem::path& path);

}