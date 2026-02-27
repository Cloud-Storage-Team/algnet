#pragma once
#include <unordered_map>

#include "i_metricable.hpp"
#include "utils/id_table.hpp"

namespace sim {
class MetricsMultiIdTable
    : public std::unordered_map<
          MetricId,
          std::unordered_map<Id, std::shared_ptr<const MetricsStorage>>> {
public:
    using std::unordered_map<
        MetricId,
        std::unordered_map<Id, std::shared_ptr<const MetricsStorage>>>::
        unordered_map;

    // Draw plot for every metric & save each to output_dir
    void draw_plots(std::filesystem::path output_dir);
};
}  // namespace sim