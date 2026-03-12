#include "send_data_action.hpp"

#include "scheduler/event/add_data_to_connection.hpp"
#include "utils/callback_observer.hpp"

namespace sim {

SendDataAction::SendDataAction(
    TimeNs a_when, SizeByte a_size, RawDataId a_raw_data_id,
    std::vector<std::shared_ptr<IConnection>> a_conns, size_t a_repeat_count,
    TimeNs a_repeat_interval, TimeNs a_jitter)
    : m_when(a_when),
      m_size(a_size),
      m_raw_data_id(std::move(a_raw_data_id)),
      m_conns(std::move(a_conns)),
      m_repeat_count(a_repeat_count),
      m_repeat_interval(a_repeat_interval),
      m_jitter(a_jitter),
      m_summary(std::make_shared<SendDataActionsSummary>()) {}

void SendDataAction::schedule() {
    const bool use_jitter = m_jitter > TimeNs(0);

    std::size_t connections_count = m_conns.size();
    std::vector<Id> connection_ids;
    connection_ids.reserve(connections_count);
    for (const auto& conn : m_conns) {
        connection_ids.emplace_back(conn->get_id());
    }

    std::uint64_t seed = std::hash<std::string>{}(m_raw_data_id);
    std::mt19937_64 rng(seed);
    for (size_t i = 0; i < m_repeat_count; ++i) {
        std::shared_ptr<SendDataActionsSummary> summary = m_summary;

        std::optional<RepeatNum> repeat_num =
            (m_repeat_count == 1 ? std::nullopt : std::make_optional(i));
        DataId data_id{m_raw_data_id, repeat_num};
        SizeByte total_size = m_size * connections_count;
        Data total_data(data_id, total_size);

        TimeNs start_time = m_when + i * m_repeat_interval;

        OnDeliveryCallback callback = [total_data, connection_ids, start_time,
                                       summary]() {
            TimeNs finish_time = Scheduler::get_instance().get_current_time();
            summary->emplace_back(total_data, connection_ids, start_time,
                                  finish_time);
        };

        std::shared_ptr<utils::CallbackObserver> observer =
            std::make_shared<utils::CallbackObserver>(connections_count,
                                                      callback);

        OnDeliveryCallback single_connection_callback = [observer]() {
            observer->on_single_callback();
        };

        Data data{data_id, m_size};
        for (auto& conn : m_conns) {
            std::uniform_int_distribution<uint64_t> dist(
                0, m_jitter.value_nanoseconds());

            TimeNs jitter_gap = use_jitter ? TimeNs(dist(rng)) : TimeNs(0);
            Scheduler::get_instance().add<AddDataToNewConnection>(
                start_time + jitter_gap, conn, data,
                single_connection_callback);
        }
    }
}

const SendDataActionsSummary& SendDataAction::get_summary() const {
    return *m_summary;
}

}  // namespace sim
