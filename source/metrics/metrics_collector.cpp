#include "metrics_collector.hpp"

#include <matplot/matplot.h>
#include <spdlog/fmt/fmt.h>

#include <filesystem>

#include "flow/interfaces/i_flow.hpp"
#include "link/i_link.hpp"
#include "utils/identifier_factory.hpp"

namespace sim {

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
                                      std::uint32_t value) {
    m_queue_size_storage.add_record(std::move(link_id), time, value);
}

void MetricsCollector::export_metrics_to_files(
    std::filesystem::path metrics_dir) const {
    m_RTT_storage.export_to_files(metrics_dir);
    m_queue_size_storage.export_to_files(metrics_dir);
    m_cwnd_storage.export_to_files(metrics_dir);
    m_rate_storage.export_to_files(metrics_dir);
}

// verctor of pairs<Storage, curve name>
using PlotMetricsData = std::vector<std::pair<MetricsStorage, std::string> >;

// Draws data from different DataStorage on one plot
static void draw_on_same_plot(std::filesystem::path path, PlotMetricsData data,
                              PlotMetadata metadata) {
    if (data.empty()) {
        return;
    }
    auto fig = matplot::figure(true);
    auto ax = fig->current_axes();
    ax->hold(matplot::on);

    for (auto& [values, name] : data) {
        values.draw_on_plot(fig, name);
    }
    ax->xlabel(metadata.x_label);
    ax->ylabel(metadata.y_label);
    ax->title(metadata.title);
    ax->legend(std::vector<std::string>());

    matplot::save(fig, path.string());
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

void MetricsCollector::draw_queue_size_plots(
    std::filesystem::path dir_path) const {
    for (auto& [link_id, values] : m_queue_size_storage.data()) {
        auto link =
            IdentifierFactory::get_instance().get_object<ILink>(link_id);
        auto fig = values.get_picture(
            {"Time, ns", "Values, bytes",
             fmt::format("Queue size from {} to {}", link->get_from()->get_id(),
                         link->get_to()->get_id())});
        auto ax = fig->current_axes();

        auto limits = ax->xlim();
        matplot::line(0, link->get_max_from_egress_buffer_size(), limits[1],
                      link->get_max_from_egress_buffer_size())
            ->line_width(1.5)
            .color({1.f, 0.0f, 0.0f});

        ax->xlim({0, limits[1]});

        ax->color("white");

        matplot::save(fig, dir_path / fmt::format("{}.svg", link_id));
    }
}

void MetricsCollector::draw_metric_plots(
    std::filesystem::path metrics_dir) const {
    draw_cwnd_plot(metrics_dir / "cwnd.svg");
    draw_delivery_rate_plot(metrics_dir / "rate.svg");
    draw_RTT_plot(metrics_dir / "rtt.svg");
    draw_queue_size_plots(metrics_dir / "queue_size");
}

void MetricsCollector::set_metrics_filter(const std::string& filter) {
    m_RTT_storage.set_filter(filter);
    m_cwnd_storage.set_filter(filter);
    m_rate_storage.set_filter(filter);
    m_queue_size_storage.set_filter(filter);
}

}  // namespace sim
