#include "metrics_collector.hpp"

#include <matplot/matplot.h>
#include <spdlog/fmt/fmt.h>

#include <filesystem>
#include <fstream>
#include <ranges>
#include <stdexcept>

namespace fs = std::filesystem;

namespace sim {

void MetricsCollector::add_RTT(Id device_id, Id flow_id, Time value) {
    if (!m_RTT_storage.contains(device_id)) {
        m_RTT_storage[device_id] = std::unordered_map<Id, std::vector<Time>>{};

    } else if (!m_RTT_storage[device_id].contains(flow_id)) {
        m_RTT_storage[device_id][flow_id] = std::vector<Time>{};
    }
    m_RTT_storage[device_id][flow_id].push_back(value);
}

void MetricsCollector::export_metrics_to_files() const {
    const std::string metrics_dir_name = "metrics";

    if (bool created = fs::create_directory(metrics_dir_name); !created) {
        if (bool is_dir_exists = fs::is_directory(metrics_dir_name);
            !is_dir_exists) {
            throw std::runtime_error("Failed to create metrics directory");
        }
    }
    if (!m_RTT_storage.empty()) {
        for (auto& [device_id, per_flow_map] : m_RTT_storage) {
            for (auto& [flow_id, values] : per_flow_map) {
                std::ofstream output_file(
                    fmt::format("metrics/RTT_{}_{}.txt", device_id, flow_id));
                if (!output_file) {
                    throw std::runtime_error(
                        "Failed to create file for RTT values");
                }
                std::ranges::copy(
                    values, std::ostream_iterator<Time>(output_file, "\n"));
                output_file.close();
            }
        }
    }
}

void MetricsCollector::draw_metric_plots() const {
    const std::string metrics_dir_name = "metrics";

    if (bool created = fs::create_directory(metrics_dir_name); !created) {
        if (bool is_dir_exists = fs::is_directory(metrics_dir_name);
            !is_dir_exists) {
            throw std::runtime_error("Failed to create metrics directory");
        }
    }
    if (!m_RTT_storage.empty()) {
        for (auto& [device_id, per_flow_map] : m_RTT_storage) {
            for (auto& [flow_id, values] : per_flow_map) {
                auto fig = matplot::figure(true);
                auto ax = fig->current_axes();

                std::vector<double> y_data(values.begin(), values.end());
                std::vector<double> x_data(y_data.size());
                std::iota(x_data.begin(), x_data.end(), 1.0);

                ax->plot(x_data, y_data, "-o")->line_width(1.5);

                ax->xlabel("");
                ax->ylabel("Value, ns");
                ax->title("RTT values");

                matplot::save(
                    fmt::format("metrics/RTT_{}_{}.png", device_id, flow_id));
            }
        }
    }
}

}  // namespace sim
