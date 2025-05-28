#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "types.hpp"

namespace sim {

class MetricsCollector {
public:
    static void init(const std::string& dir_name);
    static MetricsCollector& get_instance();

    void add_RTT(Id flow_id, Time value);
    void add_queue_size(Id link_id, std::uint32_t value);

    void export_metrics_to_files() const;
    void draw_metric_plots() const;

private:
    MetricsCollector() {}
    MetricsCollector(const MetricsCollector&) = delete;
    MetricsCollector& operator=(const MetricsCollector&) = delete;

    void create_metrics_directory() const;

    // flow_ID --> vector of RTT values
    std::unordered_map<Id, std::vector<Time>> m_RTT_storage;
    // link_ID --> vector of queue sizes
    std::unordered_map<Id, std::vector<std::uint32_t>> m_queue_size_storage;

    std::string metrics_dir_name = "metrics";
};

}  // namespace sim
