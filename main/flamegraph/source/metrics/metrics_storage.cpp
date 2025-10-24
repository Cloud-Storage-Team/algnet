#include "metrics_storage.hpp"

#include <spdlog/fmt/fmt.h>

#include "utils/safe_matplot.hpp"

namespace sim {

void MetricsStorage::add_record(TimeNs time, double value) {
    m_records.emplace_back(time, value);
}

std::vector<std::pair<TimeNs, double> > MetricsStorage::get_records() const {
    return m_records;
}

void MetricsStorage::export_to_file(std::filesystem::path path) const {
    utils::create_all_directories(path);
    std::ofstream output_file(path);
    if (!output_file) {
        throw std::runtime_error("Failed to create file for metric values");
    }
    for (const auto& pair : m_records) {
        output_file << pair.first << " " << pair.second << "\n";
    }
    output_file.close();
}

matplot::figure_handle MetricsStorage::get_picture(
    PlotMetadata metadata) const {
    auto fig = matplot::figure(true);
    draw_on_plot(fig);

    auto ax = fig->current_axes();
    ax->xlabel(metadata.x_label);
    ax->ylabel(metadata.y_label);

    ax->title(metadata.title);
    return fig;
}

void MetricsStorage::draw_plot(std::filesystem::path path,
                               PlotMetadata metadata) const {
    auto fig = get_picture(metadata);
    matplot::safe_save(fig, path.string());
}

void MetricsStorage::draw_on_plot(matplot::figure_handle& fig,
                                  std::string_view name) const {
    std::vector<double> x_data;
    std::transform(begin(m_records), end(m_records), std::back_inserter(x_data),
                   [](auto const& pair) { return pair.first.value(); });

    std::vector<double> y_data;
    std::transform(begin(m_records), end(m_records), std::back_inserter(y_data),
                   [](auto const& pair) { return pair.second; });

    auto plot = fig->current_axes()->plot(x_data, y_data, "-o");
    plot->line_width(1.5);
    plot->display_name(name);
}

}  // namespace sim
