#include "metrics_table.hpp"
#include <spdlog/fmt/fmt.h>

namespace sim {

void MetricsTable::draw_pictures(std::filesystem::path output_dir) const {
    for (const auto& [metric_id, storage] : *this) {
        std::filesystem::path plot_path =
            output_dir / fmt::format("{}.svg", metric_id.name);
        storage->draw_plot(
            plot_path,
            PlotMetadata{"Time, ns", metric_id.unit_name, metric_id.name});
    }
}

}  // namespace sim