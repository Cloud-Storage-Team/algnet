#include "metrics/metrics_collector.hpp"

#include <matplot/matplot.h>
#include <spdlog/fmt/fmt.h>

#include <algorithm>
#include <filesystem>
#include <fstream>

#include "device/receiver.hpp"
#include "device/sender.hpp"
#include "flow/flow.hpp"
#include "link.hpp"
#include "utils/identifier_factory.hpp"

namespace fs = std::filesystem;

namespace sim {

MetricsCollector& MetricsCollector::get_instance() {
    static MetricsCollector instance;
    return instance;
}

void MetricsCollector::add_RTT(Id flow_id, Time time, Time value) {
    m_RTT_storage[flow_id].add_record(time, value);
}

void MetricsCollector::add_cwnd(Id flow_id, Time time, std::uint32_t cwnd) {
    m_cwnd_storage[flow_id].add_record(time, cwnd);
}

void MetricsCollector::export_metrics_to_files(
    std::filesystem::path metrics_dir) const {
    for (auto& [flow_id, values] : m_RTT_storage) {
        values.export_to_file(metrics_dir / fmt::format("rtt_{}.txt", flow_id));
    }

    for (auto& [link_id, values] : m_queue_size_storage) {
        values.export_to_file(metrics_dir /
                              fmt::format("queue_size_{}.txt", link_id));
    }

    for (auto& [link_id, values] : m_cwnd_storage) {
        values.export_to_file(metrics_dir /
                              fmt::format("cwnd_{}.txt", link_id));
    }
}

void MetricsCollector::add_queue_size(Id link_id, Time time,
                                      std::uint32_t value) {
    m_queue_size_storage[link_id].add_record(time, value);
}

void MetricsCollector::draw_metric_plots(
    std::filesystem::path metrics_dir) const {
    for (auto& [flow_id, values] : m_RTT_storage) {
        auto flow =
            IdentifierFactory::get_instance().get_object<IFlow>(flow_id);

        values.draw_plot(
            metrics_dir / fmt::format("rtt/{}.svg", flow_id),
            PlotMetadata{"Time, ns", "Value, ns",
                         fmt::format("RTT values from {} to {}",
                                     flow->get_sender()->get_id(),
                                     flow->get_receiver()->get_id())});
    }

    for (auto& [link_id, values] : m_queue_size_storage) {
        auto link =
            IdentifierFactory::get_instance().get_object<ILink>(link_id);
        auto fig = values.get_picture(
            {"Time, ns", "Values, bytes",
             fmt::format("Queue size from {} to {}", link->get_from()->get_id(),
                         link->get_to()->get_id())});
        auto ax = fig->current_axes();

        auto limits = ax->xlim();
        matplot::line(0, link->get_max_src_egress_buffer_size_byte(), limits[1],
                      link->get_max_src_egress_buffer_size_byte())
            ->line_width(1.5)
            .color({1.f, 0.0f, 0.0f});

        ax->xlim({0, limits[1]});

        ax->color("white");

        matplot::save(fig,
                      metrics_dir / fmt::format("queue_size/{}.svg", link_id));
    }

    for (auto& [flow_id, values] : m_cwnd_storage) {
        auto flow =
            IdentifierFactory::get_instance().get_object<IFlow>(flow_id);
        values.draw_plot(metrics_dir / fmt::format("cwnd/{}.svg", flow_id),
                         {"Time, ns", "Values, packets",
                          fmt::format("Cwnd values from {} to {}",
                                      flow->get_sender()->get_id(),
                                      flow->get_receiver()->get_id())});
    }
}

}  // namespace sim
