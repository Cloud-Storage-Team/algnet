#pragma once
#include <matplot/matplot.h>

#include <filesystem>

#include "plot_metadata.hpp"
#include "types.hpp"

namespace sim {

class MetricsStorage {
public:
    void add_record(TimeNs time, double value);

    const std::vector<std::pair<TimeNs, double>>& get_records() const;
    void export_to_file(std::filesystem::path path) const;
    matplot::figure_handle get_picture(PlotMetadata metadata) const;
    void draw_plot(std::filesystem::path path, PlotMetadata metadata) const;
    void draw_on_plot(matplot::figure_handle& fig,
                      std::string_view name = "") const;

private:
    std::vector<std::pair<TimeNs, double> > m_records;
};

}  // namespace sim
