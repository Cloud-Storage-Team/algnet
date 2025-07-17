#pragma once
#include <filesystem>
#include <string>
#include <vector>

namespace sim {

struct PlotMetadata {
    std::string x_label;
    std::string y_label;
    std::string title;
};
}  // namespace sim