#include "metrics_collector.hpp"

#include <spdlog/fmt/fmt.h>

#include "connection/flow/i_flow.hpp"
#include "draw_plots.hpp"
#include "link/i_link.hpp"
#include "utils/identifier_factory.hpp"
#include "utils/safe_matplot.hpp"

namespace sim {

std::string MetricsCollector::m_metrics_filter = ".*";
bool MetricsCollector::m_is_initialised = false;

static std::string flow_id_to_curve_name(const Id& flow_id) {
    auto flow = IdentifierFactory::get_instance().get_object<IFlow>(flow_id);
    return fmt::format("{}->{}", flow->get_sender()->get_id(),
                       flow->get_receiver()->get_id());
}

MetricsCollector::MetricsCollector()
    : m_links_queue_size_storage(m_metrics_filter) {
    auto add_storage =
        [this](std::string name, PlotMetadata metadata,
               std::function<std::string(const Id&)> id_to_curve_name, bool draw_on_same_plot = true) {
            if (!m_multi_id_storages
                     .emplace(name, StorageData{MultiIdMetricsStorage(
                                                    name, m_metrics_filter), draw_on_same_plot,
                                                metadata, id_to_curve_name})
                     .second) {
                throw std::runtime_error(fmt::format(
                    "Can not initialize MetricsCollector: metric with name '{}' adds twice", name));
            }
        };

    add_storage(M_RTT_STORAGE_NAME,
                PlotMetadata{"Time, ns", "RTT, ns", "Round Trip Time"},
                flow_id_to_curve_name);

    add_storage(M_CWND_STORAGE_NAME,
                PlotMetadata{"Time, ns", "CWND, packets", "CWND"},
                flow_id_to_curve_name);
    add_storage(M_RATE_STORAGE_NAME,
                PlotMetadata{"Time, ns", "Values, Gbps", "Delivery rate"},
                flow_id_to_curve_name);
    add_storage(M_REORDERING_STORAGE_NAME,
                PlotMetadata{"Time, ns", "Reordering (inversions count)",
                             "Packet reordering"},
                flow_id_to_curve_name);

    add_storage(M_PACKET_SPACING_STORAGE_NAME,
                PlotMetadata{"Time, ns", "Packet spacing, ns",
                             "Packet spacing"},
                flow_id_to_curve_name, false);
    m_is_initialised = true;
}

MetricsCollector& MetricsCollector::get_instance() {
    static MetricsCollector instance;
    return instance;
}

MultiIdMetricsStorage& MetricsCollector::get_storage(
    const std::string& storage_name) {
    auto it = m_multi_id_storages.find(storage_name);
    if (it == m_multi_id_storages.end()) {
        throw std::runtime_error("Could not find metric " +
                                 std::string(storage_name));
    }
    return it->second.storage;
}

void MetricsCollector::add_cwnd(Id flow_id, TimeNs time, double cwnd) {
    get_storage(M_CWND_STORAGE_NAME).add_record(std::move(flow_id), time, cwnd);
}

void MetricsCollector::add_delivery_rate(Id flow_id, TimeNs time,
                                         SpeedGbps value) {
    get_storage(M_RATE_STORAGE_NAME)
        .add_record(std::move(flow_id), time, value.value());
}

void MetricsCollector::add_RTT(Id flow_id, TimeNs time, TimeNs value) {
    get_storage(M_RTT_STORAGE_NAME)
        .add_record(std::move(flow_id), time, value.value());
}

void MetricsCollector::add_packet_reordering(Id flow_id, TimeNs time,
                                             PacketReordering reordering) {
    get_storage(M_REORDERING_STORAGE_NAME)
        .add_record(flow_id, time, reordering);
}

void MetricsCollector::add_packet_spacing(Id flow_id, TimeNs time, TimeNs value) {
    get_storage(M_PACKET_SPACING_STORAGE_NAME).add_record(flow_id, time, value.value());
}

void MetricsCollector::add_queue_size(Id link_id, TimeNs time, SizeByte value,
                                      LinkQueueType type) {
    m_links_queue_size_storage.add_record(link_id, type, time, value.value());
}

void MetricsCollector::export_metrics_to_files(
    std::filesystem::path metrics_dir) const {
    for (auto [name, storage] : m_multi_id_storages) {
        storage.storage.export_to_files(metrics_dir);
    }
    m_links_queue_size_storage.export_to_files(metrics_dir);
}

void MetricsCollector::draw_queue_size_plots(
    std::filesystem::path dir_path) const {
    m_links_queue_size_storage.draw_plots(dir_path);
}

void MetricsCollector::draw_metric_plots(
    std::filesystem::path metrics_dir) const {
    for (auto [name, storage] : m_multi_id_storages) {
        if (storage.draw_on_same_plot) {
            storage.storage.draw_on_plot(metrics_dir / (name + ".svg"),
                                        storage.metadata,
                                        storage.id_to_curve_name);
        } else {
            storage.storage.draw_on_different_plots(metrics_dir / name, storage.metadata);
        }
    };
    draw_queue_size_plots(metrics_dir / "queue_size");
}

void MetricsCollector::set_metrics_filter(const std::string& filter) {
    if (m_is_initialised) {
        LOG_ERROR(fmt::format(
            "Set metrics filter {} when MetricsCollector already initialized "
            "with filter {}; no effect",
            filter, m_metrics_filter));
    }
    m_metrics_filter = filter;
}

}  // namespace sim
