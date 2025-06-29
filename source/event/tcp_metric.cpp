#include "tcp_metric.hpp"

#include "flow/interfaces/i_tcp_flow.hpp"
#include "metrics/metrics_collector.hpp"
#include "scheduler/scheduler.hpp"

namespace sim {

TcpMetric::TcpMetric(Time a_time, std::weak_ptr<ITcpFlow> a_flow)
    : Event(a_time), m_flow(a_flow) {};

void TcpMetric::operator()() {
    if (m_flow.expired()) {
        return;
    }

    auto flow = m_flow.lock();

    double cwnd = flow->get_cwnd();
    MetricsCollector::get_instance().add_cwnd(flow->get_id(), m_time, cwnd);

    Scheduler::get_instance().add<TcpMetric>(m_time + DELAY, m_flow);
}

}  // namespace sim
