#include "draw_plots.hpp"

#include "utils/safe_matplot.hpp"

namespace sim {

void draw_on_same_plot(std::filesystem::path path, PlotMetricsData data,
                       PlotMetadata metadata) {
    if (data.empty()) {
        return;
    }
    auto fig = put_on_same_plot(data, metadata);

    matplot::safe_save(fig, path.string());
}

matplot::figure_handle put_on_same_plot(PlotMetricsData data,
                                        PlotMetadata metadata) {
    auto fig = matplot::figure(true);
    auto ax = fig->current_axes();
    ax->hold(matplot::on);

    for (auto& [values, name] : data) {
        values.draw_on_plot(fig, name);
    }
    ax->xlabel(metadata.x_label);
    ax->ylabel(metadata.y_label);
    ax->title(metadata.title);
    ax->legend(std::vector<std::string>());
    return fig;
}

}  // namespace sim