#include "metrics_collector.hpp"

#include <matplot/matplot.h>
#include <spdlog/fmt/fmt.h>

#include <filesystem>
#include <fstream>
#include <numeric>
#include <ranges>
#include <stdexcept>

namespace fs = std::filesystem;

namespace sim {

void MetricsCollector::init(const std::string& dir_name) {
    get_instance().metrics_dir_name = dir_name;
}

MetricsCollector& MetricsCollector::get_instance() {
    static MetricsCollector instance;
    return instance;
}

void MetricsCollector::add_RTT(Id flow_id, Time value) {
    if (!m_RTT_storage.contains(flow_id)) {
        m_RTT_storage[flow_id] = std::vector<Time>{};
    }
    m_RTT_storage[flow_id].push_back(value);
}

void MetricsCollector::export_metrics_to_files() const {
    create_metrics_directory();
    for (auto& [flow_id, values] : m_RTT_storage) {
        std::ofstream output_file(
            fmt::format("{}/RTT_{}.txt", metrics_dir_name, flow_id));
        if (!output_file) {
            throw std::runtime_error("Failed to create file for RTT values");
        }
        std::ranges::copy(values,
                          std::ostream_iterator<Time>(output_file, "\n"));
        output_file.close();
    }
}

void MetricsCollector::draw_metric_plots() const {
    create_metrics_directory();
    for (auto& [flow_id, values] : m_RTT_storage) {
        auto fig = matplot::figure(true);
        auto ax = fig->current_axes();

        std::vector<double> y_data(values.begin(), values.end());
        std::vector<double> x_data(y_data.size());
        std::iota(x_data.begin(), x_data.end(), 1.0);

        ax->plot(x_data, y_data, "-o")->line_width(1.5);

        ax->xlabel("");
        ax->ylabel("Value, ns");
        ax->title("RTT values");

        matplot::save(fmt::format("{}/RTT_{}.png", metrics_dir_name, flow_id));
    }
}

void MetricsCollector::create_metrics_directory() const {
    if (bool created = fs::create_directory(metrics_dir_name); !created) {
        if (bool is_dir_exists = fs::is_directory(metrics_dir_name);
            !is_dir_exists) {
            throw std::runtime_error(
                fmt::format("Failed to create {} directory", metrics_dir_name));
        }
    }
}

}  // namespace sim
