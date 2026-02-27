#include "metricable_cc.hpp"

#include "cc_metrics_metadatas.hpp"
#include "scheduler.hpp"

namespace sim {

MetricableCC::MetricableCC(std::unique_ptr<ITcpCC> a_cc)
    : m_cc(std::move(a_cc)) {}

void MetricableCC::on_ack(TimeNs rtt, TimeNs avg_rtt, bool ecn_flag) {
    m_cc->on_ack(rtt, avg_rtt, ecn_flag);
    m_cwnd_storage->add_record(Scheduler::get_instance().get_current_time(),
                               m_cc->get_cwnd());
}

void MetricableCC::on_timeout() {
    m_cc->on_timeout();
    m_cwnd_storage->add_record(Scheduler::get_instance().get_current_time(),
                               m_cc->get_cwnd());
}

TimeNs MetricableCC::get_pacing_delay() const {
    return m_cc->get_pacing_delay();
}

double MetricableCC::get_cwnd() const { return m_cc->get_cwnd(); }

std::string MetricableCC::to_string() const { return m_cc->to_string(); }

// Get metrics that object collect by itself
MetricsTable MetricableCC::get_metrics_table() const {
    return MetricsTable{{CCMetricMetadatas::CWND, m_cwnd_storage}};
}

// Put metrics of all ONLY inner objects to given directory
void MetricableCC::write_inner_metrics(
    [[maybe_unused]] std::filesystem::path output_dir) const {}

}  // namespace sim