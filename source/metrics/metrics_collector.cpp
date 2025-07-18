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
      m_links_queue_size_storage(m_metrics_filter) {
    m_is_initialised = true;
}

MetricsCollector& MetricsCollector::get_instance() {
    static MetricsCollector instance;
    return instance;
}

void MetricsCollector::add_cwnd(Id flow_id, Time time, double cwnd) {
    m_cwnd_storage.add_record(std::move(flow_id), time, cwnd);
}

void MetricsCollector::add_delivery_rate(Id flow_id, Time time, double value) {
    m_rate_storage.add_record(std::move(flow_id), time, value);
}

void MetricsCollector::add_RTT(Id flow_id, Time time, Time value) {
    m_RTT_storage.add_record(std::move(flow_id), time, value);
}

void MetricsCollector::add_queue_size(Id link_id, Time time,
                                      std::uint32_t value, LinkQueueType type) {
    m_links_queue_size_storage.add_record(link_id, type, time, value);
}

void MetricsCollector::export_metrics_to_files(
    std::filesystem::path metrics_dir) const {
    m_RTT_storage.export_to_files(metrics_dir);
    m_links_queue_size_storage.export_to_files(metrics_dir);
    m_cwnd_storage.export_to_files(metrics_dir);
    m_rate_storage.export_to_files(metrics_dir);
}

void MetricsCollector::draw_cwnd_plot(std::filesystem::path path) const {
    PlotMetricsData data;
    std::transform(
        begin(m_cwnd_storage.data()), end(m_cwnd_storage.data()),
        std::back_inserter(data), [](auto const& pair) {
            auto flow =
                IdentifierFactory::get_instance().get_object<IFlow>(pair.first);
            std::string name =
                fmt::format("{}->{}", flow->get_sender()->get_id(),
                            flow->get_receiver()->get_id());
            return std::make_pair(pair.second, name);
        });
    draw_on_same_plot(path, std::move(data),
                      {"Time, ns", "CWND, packets", "CWND"});
}

void MetricsCollector::draw_delivery_rate_plot(
    std::filesystem::path path) const {
    PlotMetricsData data;
    std::transform(
        begin(m_rate_storage.data()), end(m_rate_storage.data()),
        std::back_inserter(data), [](auto const& pair) {
            auto flow =
                IdentifierFactory::get_instance().get_object<IFlow>(pair.first);
            std::string name =
                fmt::format("{}->{}", flow->get_sender()->get_id(),
                            flow->get_receiver()->get_id());
            return std::make_pair(pair.second, name);
        });
    draw_on_same_plot(path, std::move(data),
                      {"Time, ns", "Values, Gbps", "Delivery rate"});
}

void MetricsCollector::draw_RTT_plot(std::filesystem::path path) const {
    PlotMetricsData data;
    std::transform(
        begin(m_RTT_storage.data()), end(m_RTT_storage.data()),
        std::back_inserter(data), [](auto const& pair) {
            auto flow =
                IdentifierFactory::get_instance().get_object<IFlow>(pair.first);
            std::string name =
                fmt::format("{}->{}", flow->get_sender()->get_id(),
                            flow->get_receiver()->get_id());
            return std::make_pair(pair.second, name);
        });
    draw_on_same_plot(path, std::move(data),
                      {"Time, ns", "RTT, ns", "Round Trip Time"});
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
