#pragma once

#include <regex>
#include <unordered_map>

#include "link/packet_queue/link_queue.hpp"
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

    void set_metrics_filter(const std::string& filter);

private:
    MetricsCollector() {}
    MetricsCollector(const MetricsCollector&) = delete;
    MetricsCollector& operator=(const MetricsCollector&) = delete;

    void draw_cwnd_plot(std::filesystem::path path) const;
    void draw_delivery_rate_plot(std::filesystem::path path) const;
    void draw_RTT_plot(std::filesystem::path path) const;
    void draw_queue_size_plots(std::filesystem::path dir_path) const;

    // flow_ID --> vector of <time, ...> values
    MultiIdMetricsStorage m_RTT_storage = MultiIdMetricsStorage("rtt");
    MultiIdMetricsStorage m_cwnd_storage = MultiIdMetricsStorage("cwnd");
    MultiIdMetricsStorage m_rate_storage = MultiIdMetricsStorage("rate");

    // link_ID --> vector of <time, queue size> values
    MultiIdMetricsStorage m_from_egress_queue_size_storage =
        MultiIdMetricsStorage("from_egress_queue_size");

    MultiIdMetricsStorage m_to_inress_queue_size_storage =
        MultiIdMetricsStorage("to_ingress_queue_size");
};

}  // namespace sim
