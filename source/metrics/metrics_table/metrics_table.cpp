#include "metrics_table.hpp"

#include <spdlog/fmt/fmt.h>

namespace sim {

void MetricsTable::draw_pictures(std::filesystem::path output_dir) const {
#pragma omp parallel
    {
#pragma omp single
        {
            for (const auto& [metric_id, storage] : *this) {
                std::filesystem::path plot_path =
                    output_dir / fmt::format("{}.svg", metric_id.name);
#pragma omp task firstprivate(metric_id, storage, plot_path)
                {
                    storage->draw_plot(
                        plot_path, PlotMetadata{"Time, ns", metric_id.unit_name,
                                                metric_id.name});
                }
            }
#pragma omp taskwait
        }
    }
}

}  // namespace sim