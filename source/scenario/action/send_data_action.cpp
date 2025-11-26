#include "send_data_action.hpp"

#include "event/add_data_to_connection.hpp"

namespace sim {

SendDataAction::SendDataAction(TimeNs a_when, SizeByte a_size,
                               std::vector<std::weak_ptr<IConnection>> a_conns,
                               int a_repeat_count, TimeNs a_repeat_interval,
                               TimeNs a_jitter, OnDeliveryCallback a_callback)
    : m_when(a_when),
      m_size(a_size),
      m_conns(std::move(a_conns)),
      m_repeat_count(a_repeat_count),
      m_repeat_interval(a_repeat_interval),
      m_jitter(a_jitter),
      m_delivered_count(0),
      m_callback_observer(std::make_shared<CallbackObserver>(
          m_repeat_count, std::move(a_callback))) {}

void SendDataAction::schedule() {
    const bool use_jitter = m_jitter > TimeNs(0);

    std::shared_ptr<CallbackObserver> observer = m_callback_observer;

    OnDeliveryCallback single_data_callback = [observer]() {
        observer->on_single_callback();
    };

    for (auto& weak : m_conns) {
        auto conn = weak.lock();
        if (!conn) throw std::runtime_error("Expired connection in action");

        std::uint64_t seed = std::hash<std::string>{}(conn->get_id());
        std::mt19937_64 rng(seed);
        std::uniform_int_distribution<uint64_t> dist(
            0, m_jitter.value_nanoseconds());

        for (size_t i = 0; i < m_repeat_count; ++i) {
            TimeNs jitter_gap = use_jitter ? TimeNs(dist(rng)) : TimeNs(0);
            Scheduler::get_instance().add<AddDataToConnection>(
                m_when + i * m_repeat_interval + jitter_gap, conn, m_size,
                single_data_callback);
        }
    }
}

SendDataAction::CallbackObserver::CallbackObserver(
    std::size_t a_count, OnDeliveryCallback a_callback)
    : m_count(a_count), m_callbacks_triggered(0), m_callback(a_callback) {}

void SendDataAction::CallbackObserver::on_single_callback() {
    if (++m_callbacks_triggered == m_count) {
        m_callback();
    }
}

}  // namespace sim
