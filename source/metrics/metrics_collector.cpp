#include "metrics_collector.hpp"

#include <spdlog/fmt/fmt.h>

#include "draw_plots.hpp"
#include "flow/i_flow.hpp"
#include "link/i_link.hpp"
#include "utils/identifier_factory.hpp"
#include "utils/safe_matplot.hpp"

namespace sim {

std::string MetricsCollector::m_metrics_filter = ".*";
bool MetricsCollector::m_is_initialised = false;

MetricsCollector::MetricsCollector()
    : m_RTT_storage("rtt", m_metrics_filter),
      m_cwnd_storage("cwnd", m_metrics_filter),
      m_rate_storage("rate", m_metrics_filter),
      m_packet_reordering_storage("packet_reordering", m_metrics_filter),
      m_links_queue_size_storage(m_metrics_filter) {
    m_is_initialised = true;
}

MetricsCollector& MetricsCollector::get_instance() {
    static MetricsCollector instance;
    return instance;
}

void MetricsCollector::add_cwnd(Id flow_id, TimeNs time, double cwnd) {
    m_cwnd_storage.add_record(std::move(flow_id), time, cwnd);
}

void MetricsCollector::add_delivery_rate(Id flow_id, TimeNs time,
                                         SpeedGbps value) {
    m_rate_storage.add_record(std::move(flow_id), time, value.value());
}

void MetricsCollector::add_RTT(Id flow_id, TimeNs time, TimeNs value) {
    m_RTT_storage.add_record(std::move(flow_id), time, value.value());
}

void MetricsCollector::add_queue_size(Id link_id, TimeNs time, SizeByte value,
                                      LinkQueueType type) {
    m_links_queue_size_storage.add_record(link_id, type, time, value.value());
}

void MetricsCollector::add_packet_reordering(Id flow_id, TimeNs time,
                                             PacketReordering reordering) {
    m_packet_reordering_storage.add_record(flow_id, time, reordering);
}

void MetricsCollector::export_metrics_to_files(
    std::filesystem::path metrics_dir) const {
    m_cwnd_storage.export_to_files(metrics_dir);
    m_RTT_storage.export_to_files(metrics_dir);
    m_rate_storage.export_to_files(metrics_dir);
    m_packet_reordering_storage.export_to_files(metrics_dir);
    m_links_queue_size_storage.export_to_files(metrics_dir);
}

void MetricsCollector::draw_cwnd_plot(std::filesystem::path path) const {
    m_cwnd_storage.draw_on_plot(
        path, PlotMetadata{"Time, ns", "CWND, packets", "CWND"},
        [](const Id& flow_id) {
            auto flow =
                IdentifierFactory::get_instance().get_object<IFlow>(flow_id);
            return fmt::format("{}->{}", flow->get_sender()->get_id(),
                               flow->get_receiver()->get_id());
        });
}

static std::string flow_id_to_curve_name(const Id& flow_id) {
    auto flow = IdentifierFactory::get_instance().get_object<IFlow>(flow_id);
    return fmt::format("{}->{}", flow->get_sender()->get_id(),
                       flow->get_receiver()->get_id());
}

void MetricsCollector::draw_delivery_rate_plot(
    std::filesystem::path path) const {
    m_rate_storage.draw_on_plot(
        path, PlotMetadata{"Time, ns", "Values, Gbps", "Delivery rate"},
        flow_id_to_curve_name);
}

void MetricsCollector::draw_RTT_plot(std::filesystem::path path) const {
    m_RTT_storage.draw_on_plot(
        path, PlotMetadata{"Time, ns", "RTT, ns", "Round Trip Time"},
        flow_id_to_curve_name);
}

void MetricsCollector::draw_packet_reordering_plot(
    std::filesystem::path path) const {
    m_packet_reordering_storage.draw_on_plot(
        path,
        PlotMetadata{"Time, ns", "Reordering (inversions count)",
                     "Packet reordering"},
        flow_id_to_curve_name);
}

void MetricsCollector::draw_queue_size_plots(
    std::filesystem::path dir_path) const {
    m_links_queue_size_storage.draw_plots(dir_path);
}

void MetricsCollector::draw_metric_plots(
    std::filesystem::path metrics_dir) const {
    draw_cwnd_plot(metrics_dir / "cwnd.svg");
    draw_delivery_rate_plot(metrics_dir / "rate.svg");
    draw_RTT_plot(metrics_dir / "rtt.svg");
    draw_packet_reordering_plot(metrics_dir / "packet_reordering.svg");
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
