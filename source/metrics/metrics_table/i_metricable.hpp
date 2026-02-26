#pragma once
#include <filesystem>

#include "metrics_table.hpp"

namespace sim {
class IMetricable {
public:
    virtual MetricsTable get_metrics_table() const = 0;

    // Put metrics of all inner objects to given directory
    virtual void write_inner_metrics(
        std::filesystem::path output_dir) const = 0;
};
}  // namespace sim
