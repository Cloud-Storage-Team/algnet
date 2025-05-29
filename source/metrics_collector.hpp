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

    void add_RTT(Id flow_id, Time time, Time value);
    void add_queue_size(Id link_id, Time time, std::uint32_t value);

    void add_cwnd(Id flow_id, Time time, std::uint32_t cwnd);

    void export_metrics_to_files() const;
    void draw_metric_plots() const;

private:
    MetricsCollector() {}
    MetricsCollector(const MetricsCollector&) = delete;
    MetricsCollector& operator=(const MetricsCollector&) = delete;

    void create_metrics_directory() const;

    // flow_ID --> vector of <time, RTT> values
    std::unordered_map<Id, std::vector<std::pair<Time, Time>>> m_RTT_storage;
    // link_ID --> vector of <time, queue size> values
    std::unordered_map<Id, std::vector<std::pair<Time, std::uint32_t>>>
        m_queue_size_storage;
    std::unordered_map<Id, std::vector<std::pair<Time, std::uint32_t>>>
        m_cwnd_storage;

    std::string metrics_dir_name = "metrics";
};

}  // namespace sim
