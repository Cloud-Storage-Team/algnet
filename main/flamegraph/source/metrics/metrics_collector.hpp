#pragma once

#include <array>
#include <regex>
#include <unordered_map>

#include "link/packet_queue/link_queue.hpp"
#include "links_queue_size_storage.hpp"
#include "multi_id_metrics_storage.hpp"
#include "packet_reordering/i_packet_reordering.hpp"
namespace sim {

struct StorageData {
    MultiIdMetricsStorage storage;
    PlotMetadata metadata;
    std::function<std::string(const Id&)> id_to_curve_name;
    bool draw_on_same_plot;
};

class MetricsCollector {
public:
    static MetricsCollector& get_instance();

    // Flow metrics
    void add_cwnd(Id flow_id, TimeNs time, double cwnd);
    void add_delivery_rate(Id flow_id, TimeNs time, SpeedGbps value);
    void add_RTT(Id flow_id, TimeNs time, TimeNs value);
    void add_packet_reordering(Id flow_id, TimeNs time, PacketReordering value);
    void add_packet_spacing(Id flow_id, TimeNs time, TimeNs value);

    // Link metrics
    void add_queue_size(Id link_id, TimeNs time, SizeByte value,
                        LinkQueueType type = LinkQueueType::FromEgress);

    // Layout
    void export_metrics_to_files(std::filesystem::path metrics_dir) const;
    void draw_metric_plots(std::filesystem::path metrics_dir) const;

    static void set_metrics_filter(const std::string& filter);

private:
    static std::string m_metrics_filter;
    static bool m_is_initialised;

    MetricsCollector();
    MetricsCollector(const MetricsCollector&) = delete;
    MetricsCollector& operator=(const MetricsCollector&) = delete;

    void draw_queue_size_plots(std::filesystem::path dir_path) const;

    MultiIdMetricsStorage& get_storage_named(const std::string& name);

    static constexpr std::string M_RTT_STORAGE_NAME = "rtt";
    static constexpr std::string M_CWND_STORAGE_NAME = "cwnd";
    static constexpr std::string M_RATE_STORAGE_NAME = "rate";
    static constexpr std::string M_REORDERING_STORAGE_NAME = "reordering";
    static constexpr std::string M_PACKET_SPACING_STORAGE_NAME =
        "packet_spacing";

    std::unordered_map<std::string, StorageData> m_multi_id_storages;

    // link_ID --> vector of <time, queue size> values
    LinksQueueSizeStorage m_links_queue_size_storage;
};

}  // namespace sim
