#include "scenario/scenario_impl.hpp"

#include "logger/logger.hpp"
#include "scheduler.hpp"

namespace sim {

void ScenarioImpl::add_send_data_conn_batch(SendDataConnBatch batch) {
    m_send_batches.emplace_back(std::move(batch));
}

void ScenarioImpl::start() {
    for (const auto& b : m_send_batches) {
        if (b.conns.empty()) {
            throw std::runtime_error(
                "send_data_conn batch has no target connections");
        }
        for (const auto& weak_conn : b.conns) {
            auto conn = weak_conn.lock();
            if (!conn) {
                throw std::runtime_error(
                    "Connection expired in scenario; cannot schedule "
                    "send_data_conn");
            }
            for (uint64_t i = 0; i < b.num; ++i) {
                const TimeNs t = b.when + static_cast<int64_t>(i) * b.gap;
                Scheduler::get_instance().add<AddDataToConnection>(t, conn,
                                                                   b.size);
            }
        }
    }
}

}  // namespace sim
