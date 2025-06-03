#pragma once
#include <matplot/matplot.h>

#include <filesystem>
#include <tuple>
#include <vector>

#include "types.hpp"

namespace sim {

using MetricsValue = long long;

struct PlotMetadata {
    std::string x_label;
    std::string y_label;
    std::string title;
};

class MetricsStorage {
public:
    void add_record(Time time, MetricsValue value);

    void export_to_file(std::filesystem::path path) const;
    matplot::figure_handle get_picture(PlotMetadata metadata) const;
    void draw_plot(std::filesystem::path path, PlotMetadata metadata) const;

private:
    std::vector<std::pair<Time, MetricsValue> > m_records;
};

}  // namespace sim
