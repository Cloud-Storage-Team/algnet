#pragma once
#include "../cc/metricable_cc.hpp"
#include "../i_mplb.hpp"
#include "../path_chooser/i_path_chooser.hpp"
#include "metrics/fairness/fairness.hpp"
#include "metrics/metrics_storage.hpp"

namespace sim {

struct SingleCCMetricsFilters {
    bool fairness = true;
};

class SingleCCMplb : public IMPLB,
                     public std::enable_shared_from_this<SingleCCMplb> {
public:
    virtual ~SingleCCMplb() = default;

    static constexpr inline SingleCCMetricsFilters DEFAULT_METRICS_FILTERS = {};

    static std::shared_ptr<SingleCCMplb> create_shared(
        MetricableCC a_cc, std::unique_ptr<IPathChooser> a_path_chooser,
        SizeByte a_packet_size = SizeByte(1500),
        SingleCCMetricsFilters a_metrics_filters = DEFAULT_METRICS_FILTERS);

    [[nodiscard]] virtual utils::StrExpected<void> send_data(
        Data data, OnDeliveryCallback callback) final;

    virtual MPLBContext get_context() const final;

    virtual MetricsTable get_metrics_table() const final;

    // Put metrics of all inner objects to given directory
    virtual void write_inner_metrics(
        std::filesystem::path output_dir) const final;

private:
    SingleCCMplb(MetricableCC a_cc,
                 std::unique_ptr<IPathChooser> a_path_chooser,
                 SizeByte a_packet_size,
                 SingleCCMetricsFilters a_metrics_filters);
    SizeByte get_quota() const;

    MetricableCC m_cc;

    SizeByte m_sent_data_size;
    SizeByte m_delivered_data_size;
    std::size_t m_packets_in_flight;

    std::unique_ptr<IPathChooser> m_path_chooser;
    SizeByte m_packet_size;

    // fairness for flows
    JainsFairnessIndex<SpeedGbps> m_delivery_rate_fairness;

    std::shared_ptr<MetricsStorage> m_fairness_storage =
        std::make_shared<MetricsStorage>();

    SingleCCMetricsFilters m_metrics_filters;
};

}  // namespace sim
