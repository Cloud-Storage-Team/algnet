#include "metricable_cc.hpp"

#include "cc_metrics_metadatas.hpp"
#include "scheduler.hpp"

namespace sim {

MetricableCC::MetricableCC(std::unique_ptr<ITcpCC> a_cc)
    : m_cc(std::move(a_cc)) {}

void MetricableCC::on_ack(TimeNs rtt, TimeNs avg_rtt, bool ecn_flag) {
    m_cc->on_ack(rtt, avg_rtt, ecn_flag);
    record_cwnd();
}

void MetricableCC::on_timeout() {
    m_cc->on_timeout();
    record_cwnd();
}

TimeNs MetricableCC::get_pacing_delay() const {
    return m_cc->get_pacing_delay();
}

double MetricableCC::get_cwnd() const { return m_cc->get_cwnd(); }

std::string MetricableCC::to_string() const { return m_cc->to_string(); }

MetricsTable MetricableCC::get_metrics_table() const {
    return MetricsTable{{CCMetricMetadatas::CWND, m_cwnd_storage}};
}

void MetricableCC::write_inner_metrics(
    [[maybe_unused]] std::filesystem::path output_dir) const {}

void MetricableCC::record_cwnd() const {
    m_cwnd_storage->add_record(Scheduler::get_instance().get_current_time(),
                               m_cc->get_cwnd());
}

}  // namespace sim