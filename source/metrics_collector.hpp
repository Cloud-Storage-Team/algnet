#pragma once

#include <unordered_map>
#include <vector>

#include "types.hpp"

namespace sim {

class MetricsCollector {
public:
    static MetricsCollector& get_instance();

    void add_RTT(Id flow_id, Time value);

    void export_metrics_to_files() const;
    void draw_metric_plots() const;

private:
    MetricsCollector() {}
    MetricsCollector(const MetricsCollector&) = delete;
    MetricsCollector& operator=(const MetricsCollector&) = delete;

    void create_metrics_directory() const;

    // flow_ID --> vector of RTT values
    std::unordered_map<Id, std::vector<Time>> m_RTT_storage;

    std::string metrics_dir_name = "metrics";
};

}  // namespace sim
