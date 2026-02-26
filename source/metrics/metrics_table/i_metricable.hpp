#pragma once
#include <filesystem>

#include "metrics_table.hpp"

namespace sim {
class IMetricable {
public:
    // Get metrics that object collect by itself
    virtual MetricsTable get_metrics_table() const = 0;

    // Put metrics of all ONLY inner objects to given directory
    virtual void write_inner_metrics(
        std::filesystem::path output_dir) const = 0;

    void write_all_metrics(std::filesystem::path output_dir) const {
        get_metrics_table().draw_pictures(output_dir);
        write_inner_metrics(output_dir);
    }
};
}  // namespace sim
