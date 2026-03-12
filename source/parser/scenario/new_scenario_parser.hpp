#pragma once
#include <filesystem>

#include "scenario/new_scenario.hpp"

namespace sim {

NewScenario parse_scenario(const std::filesystem::path& path);

}
