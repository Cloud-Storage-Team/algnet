#pragma once

#include <regex>
#include <unordered_map>

#include "link/packet_queue/link_queue.hpp"
#include "links_queue_size_storage.hpp"
#include "multi_id_metrics_storage.hpp"
namespace sim {

class MetricsCollector {
public:
    static MetricsCollector& get_instance();

    void add_cwnd(Id flow_id, Time time, double cwnd);
    void add_delivery_rate(Id flow_id, Time time, double value);
    void add_RTT(Id flow_id, Time time, Time value);
    void add_queue_size(Id link_id, Time time, std::uint32_t value,
                        LinkQueueType type = LinkQueueType::FromEgress);

    void export_metrics_to_files(std::filesystem::path metrics_dir) const;
    void draw_metric_plots(std::filesystem::path metrics_dir) const;

    static void set_metrics_filter(const std::string& filter);

private:
    MetricsCollector();
    MetricsCollector(const MetricsCollector&) = delete;
    MetricsCollector& operator=(const MetricsCollector&) = delete;

    void draw_cwnd_plot(std::filesystem::path path) const;
    void draw_delivery_rate_plot(std::filesystem::path path) const;
    void draw_RTT_plot(std::filesystem::path path) const;
    void draw_queue_size_plots(std::filesystem::path dir_path) const;

    static std::string m_metrics_filter;
    static bool m_is_initialised;

    // flow_ID --> vector of <time, ...> values
    MultiIdMetricsStorage m_RTT_storage;
    MultiIdMetricsStorage m_cwnd_storage;
    MultiIdMetricsStorage m_rate_storage;

    // link_ID --> vector of <time, queue size> values
    LinksQueueSizeStorage m_links_queue_size_storage;
};

}  // namespace sim
