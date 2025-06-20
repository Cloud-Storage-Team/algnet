#pragma once

#include <unordered_map>

#include "metrics/metrics_storage.hpp"

namespace sim {

class MetricsCollector {
public:
    static MetricsCollector& get_instance();

    void add_cwnd(Id flow_id, Time time, double cwnd);
    void add_delivery_rate(Id flow_id, Time time, double value);
    void add_RTT(Id flow_id, Time time, Time value);
    void add_queue_size(Id link_id, Time time, std::uint32_t value);

    void export_metrics_to_files(std::filesystem::path metrics_dir) const;
    void draw_metric_plots(std::filesystem::path metrics_dir) const;

private:
    MetricsCollector() {}
    MetricsCollector(const MetricsCollector&) = delete;
    MetricsCollector& operator=(const MetricsCollector&) = delete;

    void draw_cwnd_plot(std::filesystem::path path) const;
    void draw_delivery_rate_plot(std::filesystem::path path) const;
    void draw_RTT_plot(std::filesystem::path path) const;
    void draw_queue_size_plots(std::filesystem::path dir_path) const;

    // flow_ID --> vector of <time, ...> values
    std::unordered_map<Id, MetricsStorage> m_RTT_storage;
    std::unordered_map<Id, MetricsStorage> m_cwnd_storage;
    std::unordered_map<Id, MetricsStorage> m_rate_storage;

    // link_ID --> vector of <time, queue size> values
    std::unordered_map<Id, MetricsStorage> m_queue_size_storage;
};

}  // namespace sim
