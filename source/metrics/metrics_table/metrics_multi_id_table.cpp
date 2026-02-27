#include "metrics_multi_id_table.hpp"

#include "utils/safe_matplot.hpp"

namespace sim {

void MetricsMultiIdTable::draw_plots(std::filesystem::path output_dir) {
    const static std::size_t MAX_LEGENG_SIZE = 5;
    for (const auto& [metric_name, multi_id_storage] : *this) {
        std::filesystem::path metric_output_path =
            output_dir / fmt::format("{}.svg", metric_name.name);

        auto fig = matplot::figure(true);
        auto ax = fig->current_axes();
        ax->hold(matplot::on);

        for (const auto& [id, values] : multi_id_storage) {
            values->draw_on_plot(fig, id);
        }

        ax->xlabel("Time, nanoseconds");
        ax->ylabel(metric_name.unit_name);
        ax->title(metric_name.name);

        if (multi_id_storage.size() <= MAX_LEGENG_SIZE) {
            ax->legend(std::vector<std::string>());
        }

        matplot::safe_save(fig, metric_output_path);
    }
}

}  // namespace sim