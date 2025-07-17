#include "metrics_collector.hpp"

#include <spdlog/fmt/fmt.h>

#include "flow/i_flow.hpp"
#include "link/i_link.hpp"
#include "utils/identifier_factory.hpp"
#include "utils/safe_matplot.hpp"

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
                                      std::uint32_t value, LinkQueueType type) {
    switch (type) {
        case LinkQueueType::FromEgress: {
            m_from_egress_queue_size_storage.add_record(std::move(link_id),
                                                        time, value);
            break;
        }
        case LinkQueueType::ToIngress: {
            m_to_inress_queue_size_storage.add_record(std::move(link_id), time,
                                                      value);
            break;
        }
        default: {
            LOG_ERROR(fmt::format("Unexpected LinkQueueSize {}",
                                  static_cast<int>(type)));
        }
    }
}

void MetricsCollector::export_metrics_to_files(
    std::filesystem::path metrics_dir) const {
    m_RTT_storage.export_to_files(metrics_dir);
    m_from_egress_queue_size_storage.export_to_files(metrics_dir);
    m_cwnd_storage.export_to_files(metrics_dir);
    m_rate_storage.export_to_files(metrics_dir);
}

// verctor of pairs<Storage, curve name>
using PlotMetricsData = std::vector<std::pair<MetricsStorage, std::string> >;

// Puts data from different DataStorage on one plot
static matplot::figure_handle put_on_same_plot(PlotMetricsData data,
                                               PlotMetadata metadata) {
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
    return fig;
}

// Draws data from different DataStorage on one plot
static void draw_on_same_plot(std::filesystem::path path, PlotMetricsData data,
                              PlotMetadata metadata) {
    if (data.empty()) {
        return;
    }
    auto fig = put_on_same_plot(data, metadata);

    matplot::safe_save(fig, path.string());
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
    // for data from both from_ingress and to_ingress queue sizes
    std::map<Id, PlotMetricsData> queue_size_data;
    for (auto& [link_id, values] : m_from_egress_queue_size_storage.data()) {
        auto link =
            IdentifierFactory::get_instance().get_object<ILink>(link_id);
        std::string curve_name =
            fmt::format("{} engress queue size", link->get_from()->get_id());
        queue_size_data[link_id].emplace_back(values, curve_name);
    }

    for (auto& [link_id, values] : m_to_inress_queue_size_storage.data()) {
        auto link =
            IdentifierFactory::get_instance().get_object<ILink>(link_id);
        std::string curve_name =
            fmt::format("{} ingress queue size", link->get_to()->get_id());
        queue_size_data[link_id].emplace_back(values, curve_name);
    }
    for (auto [link_id, data] : queue_size_data) {
        auto link =
            IdentifierFactory::get_instance().get_object<ILink>(link_id);
        PlotMetadata metadata = {
            "Time, ns", "Values, bytes",
            fmt::format("Queue size from {} to {}", link->get_from()->get_id(),
                        link->get_to()->get_id())};

        auto fig = put_on_same_plot(data, metadata);
        auto ax = fig->current_axes();

        auto limits = ax->xlim();

        auto draw_gorizontal_line =
            [&limits](double line_y, std::initializer_list<float> color) {
                matplot::line(0, line_y, limits[1], line_y)
                    ->line_width(1.5)
                    .color(color);
            };

        draw_gorizontal_line(link->get_max_from_egress_buffer_size(),
                             {1.f, 0.f, 0.f});
        draw_gorizontal_line(link->get_max_to_ingress_queue_size(),
                             {0.f, 1.f, 0.f});

        // matplot::line(0, link->get_max_from_egress_buffer_size(), limits[1],
        //               link->get_max_from_egress_buffer_size())
        //     ->line_width(1.5)
        //     .color({1.f, 0.0f, 0.0f});

        // matplot::line(0, link->get_max_to_ingress_queue_size(), limits[1],
        //               link->get_max_to_ingress_queue_size())
        //     ->line_width(1.5)
        //     .color({1.f, 0.0f, 0.0f});

        ax->xlim({0, limits[1]});
        ax->color("white");

        std::filesystem::path plot_path =
            dir_path / fmt::format("{}.svg", link_id);

        matplot::safe_save(fig, plot_path.string());
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
    m_from_egress_queue_size_storage.set_filter(filter);
}

}  // namespace sim
