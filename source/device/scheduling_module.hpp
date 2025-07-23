#pragma once

#include "logger/logger.hpp"
#include "scheduler.hpp"

namespace sim {

template <typename TDevice, typename TEvent>
class SchedulingModule {
public:
    // increment counter; return true if counter = 1
    bool notify_about_arriving(TimeNs arrival_time,
                               std::weak_ptr<TDevice> subject) {
        m_cnt++;
        bool result = (m_cnt == 1);
        if (result) {
            reschedule_event(arrival_time, subject);
        }

        return result;
    };

    // decrement counter, update earliest_possible_time; return true if counter
    // = 0
    bool notify_about_finish(TimeNs finish_time) {
        if (m_cnt == 0) {
            LOG_CRITICAL(
                "Impossible sittuation: notify_about_finish triggered, but "
                "counter is zero");
            return false;
        }
        m_cnt--;
        m_earliest_possible_time = finish_time;

        return (m_cnt == 0);
    };

private:
    void reschedule_event(TimeNs preferred_processing_time,
                          std::weak_ptr<TDevice> target) {
        m_earliest_possible_time =
            std::max(m_earliest_possible_time, preferred_processing_time);

        Scheduler::get_instance().add<TEvent>(m_earliest_possible_time, target);
    }

    std::uint32_t m_cnt = 0;
    TimeNs m_earliest_possible_time = TimeNs(0);
};

}  // namespace sim
