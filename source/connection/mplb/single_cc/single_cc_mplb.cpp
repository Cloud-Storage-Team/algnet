#include "single_cc_mplb.hpp"

#include <cmath>

#include "../mplb_metrics_metadatas.hpp"
#include "connection/flow/new_flows_summary/new_flows_summary.hpp"
#include "event/call_at_time.hpp"
#include "event/send_data.hpp"
#include "metrics/metrics_table/combine_metrics_tables.hpp"
#include "scheduler.hpp"
#include "utils/callback_observer.hpp"

namespace sim {

std::shared_ptr<SingleCCMplb> SingleCCMplb::create_shared(
    MetricableCC a_cc, std::unique_ptr<IPathChooser> a_path_chooser,
    SizeByte a_packet_size, SingleCCMetricsFilters a_metrics_filters) {
    return std::shared_ptr<SingleCCMplb>(
        new SingleCCMplb(std::move(a_cc), std::move(a_path_chooser),
                         a_packet_size, std::move(a_metrics_filters)));
}

SingleCCMplb::SingleCCMplb(MetricableCC a_cc,
                           std::unique_ptr<IPathChooser> a_path_chooser,
                           SizeByte a_packet_size,
                           SingleCCMetricsFilters a_metrics_filters)
    : m_cc(std::move(a_cc)),
      m_sent_data_size(0),
      m_delivered_data_size(0),
      m_packets_in_flight(0),
      m_path_chooser(std::move(a_path_chooser)),
      m_packet_size(a_packet_size),
      m_delivery_rate_fairness(m_path_chooser->get_flows_table()),
      m_metrics_filters(std::move(a_metrics_filters)) {}

utils::StrExpected<void> SingleCCMplb::send_data(Data data,
                                                 OnDeliveryCallback callback) {
    SizeByte quota = get_quota();
    if (data.size > quota) {
        return std::unexpected(
            fmt::format("Data size {}b is bigger than quota size: {}b",
                        data.size.value(), quota.value()));
    }

    TimeNs pacing_delay = m_cc.get_pacing_delay();
    TimeNs shift(0);

    std::size_t packets_count =
        (data.size + m_packet_size - SizeByte(1)) / m_packet_size;

    std::shared_ptr<utils::CallbackObserver> observer =
        std::make_shared<utils::CallbackObserver>(packets_count, callback);
    TimeNs now = Scheduler::get_instance().get_current_time();

    for (size_t packet_num = 0; packet_num < packets_count; packet_num++) {
        // TODO: think about pacing delay & sending data to many flows (is it
        // really needed in such case?)
        std::shared_ptr<INewFlow> flow = m_path_chooser->choose_flow();
        shift += pacing_delay;
        std::shared_ptr<SingleCCMplb> mplb = shared_from_this();
        PacketCallback packet_callback = [observer, mplb,
                                          flow](PacketAckInfo info) {
            mplb->m_cc.on_ack(info.rtt, info.avg_rtt, info.ecn_flag);
            mplb->m_packets_in_flight--;
            mplb->m_delivered_data_size += mplb->m_packet_size;
            observer->on_single_callback();
            std::optional<SpeedGbps> opt_delivery_rate =
                flow->get_context().delivery_rate_statistics.get_last();
            Id flow_id = flow->get_id();
            if (opt_delivery_rate.has_value()) {
                double fairness = mplb->m_delivery_rate_fairness.update(
                    flow_id, opt_delivery_rate.value());

                mplb->m_fairness_storage->add_record(
                    Scheduler::get_instance().get_current_time(), fairness);
            } else {
                LOG_WARN(
                    fmt::format("Delivery rate statistics of flow {} does not "
                                "have last value on packet delivery",
                                flow_id));
            }
        };
        PacketInfo info{data.id, m_packet_size, packet_callback, now};
        Scheduler::get_instance().add<CallAtTime>(
            now + shift, [mplb, flow, packet_info = std::move(info)]() {
                mplb->m_sent_data_size += packet_info.packet_size;
                flow->send(std::vector<PacketInfo>({std::move(packet_info)}));
            });
        m_packets_in_flight++;
    }

    return {};
}

MetricsTable SingleCCMplb::get_metrics_table() const {
    MetricsTable metrics_table;
    if (m_metrics_filters.fairness) {
        metrics_table.emplace(MplbMetricMetadatas::FAIRNESS,
                              m_fairness_storage);
    }
    return metrics_table;
}

void SingleCCMplb::write_inner_metrics(
    std::filesystem::path output_dir_path) const {
    std::filesystem::path flows_output_path = output_dir_path / "flows";
    const auto& flows_table = m_path_chooser->get_flows_table();
    collect_and_save_all_metrics(flows_table, flows_output_path);

    NewFlowsSummary(flows_table)
        .write_to_csv(flows_output_path / "summary.csv");

    m_cc.write_all_metrics(output_dir_path / "cc");
}

MPLBContext SingleCCMplb::get_context() const {
    return MPLBContext{m_sent_data_size, m_delivered_data_size, get_quota()};
}

SizeByte SingleCCMplb::get_quota() const {
    const double cwnd = m_cc.get_cwnd();

    // Effective window: the whole part of cwnd; if cwnd < 1 and inflight ==
    // 0, allow 1 packet
    std::uint32_t effective_cwnd = static_cast<std::uint32_t>(std::floor(cwnd));
    if (m_packets_in_flight == 0 && cwnd < 1.0) {
        effective_cwnd = 1;
    }
    const std::uint32_t quota_pkts =
        (effective_cwnd > m_packets_in_flight)
            ? (effective_cwnd - m_packets_in_flight)
            : 0;

    // Quota in bytes, multiple of the packet size
    return quota_pkts * m_packet_size;
}

}  // namespace sim
