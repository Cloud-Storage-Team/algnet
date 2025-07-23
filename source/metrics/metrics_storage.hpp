#pragma once
#include <matplot/matplot.h>

#include <filesystem>
#include <vector>

#include "types.hpp"

namespace sim {

struct PlotMetadata {
    std::string x_label;
    std::string y_label;
    std::string title;
};

class MetricsStorage {
public:
    void add_record(TimeNs time, double value);

    void export_to_file(std::filesystem::path path) const;
    matplot::figure_handle get_picture(PlotMetadata metadata) const;
    void draw_plot(std::filesystem::path path, PlotMetadata metadata) const;
    void draw_on_plot(matplot::figure_handle& fig,
                      std::string_view name = "") const;

private:
    std::vector<std::pair<TimeNs, double> > m_records;
};

}  // namespace sim
