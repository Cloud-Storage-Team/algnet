#include "metrics_storage.hpp"

#include <spdlog/fmt/fmt.h>

namespace sim {

void MetricsStorage::add_record(Time time, MetricsValue value) {
    m_records.emplace_back(time, value);
}

void static create_all_directories(std::filesystem::path path) {
    std::filesystem::path dir_path = path.parent_path();
    if (std::filesystem::exists(dir_path)) {
        return;
    }
    if (!std::filesystem::create_directories(dir_path) ||
        !std::filesystem::exists(dir_path)) {
        throw std::runtime_error(
            fmt::format("Can not create {} directory", dir_path.string()));
    }
}

void MetricsStorage::export_to_file(std::filesystem::path path) const {
    create_all_directories(path);
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
    auto ax = fig->current_axes();

    std::vector<double> x_data;
    std::transform(begin(m_records), end(m_records), std::back_inserter(x_data),
                   [](auto const& pair) { return pair.first; });

    std::vector<double> y_data;
    std::transform(begin(m_records), end(m_records), std::back_inserter(y_data),
                   [](auto const& pair) { return pair.second; });

    ax->plot(x_data, y_data, "-o")->line_width(1.5);

    ax->xlabel(metadata.x_label);
    ax->ylabel(metadata.y_label);

    ax->title(metadata.title);
    return fig;
}

void MetricsStorage::draw_plot(std::filesystem::path path,
                               PlotMetadata metadata) const {
    auto fig = get_picture(metadata);
    create_all_directories(path);
    matplot::save(fig, path.string());
}

}  // namespace sim
