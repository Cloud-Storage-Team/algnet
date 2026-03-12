#pragma once
#include <filesystem>

#include "scenario/scenario.hpp"

namespace sim {

NewScenario parse_scenario(const std::filesystem::path& path);

}
