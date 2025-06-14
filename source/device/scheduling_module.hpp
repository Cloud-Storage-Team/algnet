#pragma once

#include "scheduler.hpp"
#include "logger/logger.hpp"

namespace sim {

template<typename TDevice, typename TEvent>
class SchedulingModule {
public:
    // increment counter; return true if counter = 1
    bool notify_about_arriving(Time arrival_time, std::weak_ptr<TDevice> subject) {
        m_cnt++;
        bool result = (m_cnt == 1);
        if (result) {
            reschedule_event(arrival_time, subject);
        }

        return result;
    };
    
    // decrement counter, update earliest_possible_time; return true if counter = 0
    bool notify_about_finish(Time finish_time) { 
        if (m_cnt == 0) {
            LOG_CRITICAL("Impossible sittuation: notify_about_finish triggered, but counter is zero");
            return false;
        }
        m_cnt--;
        m_earliest_possible_time = finish_time;

        return (m_cnt == 0);
    };

private:
    void reschedule_event(Time preferred_processing_time, std::weak_ptr<TDevice> target) {
        m_earliest_possible_time = std::max(m_earliest_possible_time, preferred_processing_time);

        std::unique_ptr<Event> new_event = std::make_unique<TEvent>(m_earliest_possible_time, target);
        Scheduler::get_instance().add(std::move(new_event));
    }

    std::uint32_t m_cnt = 0;
    Time m_earliest_possible_time = 0;
};

}  // namespace sim
