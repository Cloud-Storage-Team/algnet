#include "send_data_action.hpp"

#include "event/add_data_to_connection.hpp"

namespace sim {

SendDataAction::SendDataAction(TimeNs a_when, SizeByte a_size,
                               std::vector<std::weak_ptr<IConnection>> a_conns,
                               int a_repeat_count, TimeNs a_repeat_interval)
    : m_when(a_when),
      m_size(a_size),
      m_conns(std::move(a_conns)),
      m_repeat_count(a_repeat_count),
      m_repeat_interval(a_repeat_interval) {}

void SendDataAction::schedule() {
    for (auto& weak : m_conns) {
        auto conn = weak.lock();
        if (!conn) throw std::runtime_error("Expired connection in action");

        for (size_t i = 0; i < m_repeat_count; ++i) {
            Scheduler::get_instance().add<AddDataToConnection>(
                m_when + i * m_repeat_interval, conn, m_size);
        }
    }
}

}  // namespace sim
