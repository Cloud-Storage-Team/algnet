#pragma once
#include <memory>

#include "i_tcp_cc.hpp"
#include "metrics/metrics_storage.hpp"
#include "metrics/metrics_table/i_metricable.hpp"

namespace sim {

// Wrapper of std::unique_ptr<ITcpCC> that supports metrics collecting
class MetricableCC : public ITcpCC, public IMetricable {
public:
    explicit MetricableCC(std::unique_ptr<ITcpCC> a_cc);

    virtual void on_ack(TimeNs rtt, TimeNs avg_rtt, bool ecn_flag) final;

    virtual void on_timeout() final;

    virtual TimeNs get_pacing_delay() const final;

    virtual double get_cwnd() const final;

    virtual std::string to_string() const final;

    // Get metrics that object collect by itself
    virtual MetricsTable get_metrics_table() const final;

    // Put metrics of all ONLY inner objects to given directory
    virtual void write_inner_metrics(
        std::filesystem::path output_dir) const final;

private:
    void record_cwnd() const;

    std::unique_ptr<ITcpCC> m_cc;
    std::shared_ptr<MetricsStorage> m_cwnd_storage =
        std::make_shared<MetricsStorage>();
};

}  // namespace sim