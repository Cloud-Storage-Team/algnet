#pragma once
#include <matplot/matplot.h>

#include "metrics_storage.hpp"

namespace sim {

using PlotMetricsData = std::vector<std::pair<MetricsStorage, std::string> >;

// Puts data from different DataStorage on one plot
matplot::figure_handle put_on_same_plot(PlotMetricsData data,
                                        PlotMetadata metadata);

// Draws data from different DataStorage on one plot
void draw_on_same_plot(std::filesystem::path path, PlotMetricsData data,
                       PlotMetadata metadata);
}  // namespace sim
