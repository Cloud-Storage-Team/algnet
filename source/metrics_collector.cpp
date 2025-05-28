#include "metrics_collector.hpp"

#include <matplot/matplot.h>
#include <spdlog/fmt/fmt.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
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

void MetricsCollector::add_RTT(Id flow_id, Time time, Time value) {
    if (!m_RTT_storage.contains(flow_id)) {
        m_RTT_storage[flow_id] = std::vector<std::pair<Time, Time>>{};
    }
    m_RTT_storage[flow_id].emplace_back(time, value);
}

void MetricsCollector::export_metrics_to_files() const {
    create_metrics_directory();
    for (auto& [flow_id, values] : m_RTT_storage) {
        std::ofstream output_file(fmt::format("{}/RTT_{}.txt", metrics_dir_name, flow_id));
        if (!output_file) {
            throw std::runtime_error("Failed to create file for RTT values");
        }
        for (const auto& pair : values) {
            output_file << pair.first << " " << pair.second << "\n";
        }
        output_file.close();
    }
    // TODO: Add queue size export
}

void MetricsCollector::add_queue_size(Id link_id, Time time, std::uint32_t value) {
    if (!m_queue_size_storage.contains(link_id)) {
        m_queue_size_storage[link_id] = std::vector<std::pair<Time, std::uint32_t>>{};
    }
    m_queue_size_storage[link_id].emplace_back(time, value);
}

void MetricsCollector::draw_metric_plots() const {
    create_metrics_directory();
    for (auto& [flow_id, values] : m_RTT_storage) {
        auto fig = matplot::figure(true);
        auto ax = fig->current_axes();

        std::vector<double> x_data;
        std::transform(begin(values), end(values),
               std::back_inserter(x_data),
               [](auto const& pair){ return pair.second; });        
        
        std::vector<double> y_data;
        std::transform(begin(values), end(values),
               std::back_inserter(y_data),
               [](auto const& pair){ return pair.second; });

        ax->plot(x_data, y_data, "-o")->line_width(1.5);

        ax->xlabel("Time, ns");
        ax->ylabel("Value, ns");
        ax->title("RTT values");

        matplot::save(fmt::format("{}/RTT_{}.png", metrics_dir_name, flow_id));
    }

    for (auto& [link_id, values] : m_queue_size_storage) {
        auto fig = matplot::figure(true);
        auto ax = fig->current_axes();

        // std::vector<double> y_data(values.begin(), values.end());
        // std::vector<double> x_data(y_data.size());
        // std::iota(x_data.begin(), x_data.end(), 1.0);

        std::vector<double> x_data;
        std::transform(begin(values), end(values),
               std::back_inserter(x_data),
               [](auto const& pair){ return pair.second; });        
        
        std::vector<double> y_data;
        std::transform(begin(values), end(values),
               std::back_inserter(y_data),
               [](auto const& pair){ return pair.second; });

        ax->plot(x_data, y_data, "-o")->line_width(1.5);

        ax->xlabel("Time, ns");
        ax->ylabel("Value, packets");
        ax->title("Queue size");

        matplot::save(fmt::format("{}/queue_size_{}.png", metrics_dir_name, link_id));
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
