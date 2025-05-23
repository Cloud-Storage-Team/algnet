#include "scheduling_module.hpp"

#include <algorithm>
#include <memory>

#include "event.hpp"
#include "scheduler.hpp"
#include "logger/logger.hpp"

namespace sim {

bool notify_about_finish(Time finish_time, std::uint32_t& cnt, Time& earliest_possible_time) {
    if (cnt == 0) {
        // TODO warning, impossible situation
        return false;
    }
    cnt--;
    earliest_possible_time = finish_time;

    return (cnt == 0);
};

template<typename T, typename E>
bool notify_about_arriving(Time arrival_time, std::weak_ptr<T> target, std::uint32_t& cnt, Time& earliest_possible_time) {
    cnt++;
    bool result = (cnt == 1);
    if (result) {
        reschedule_event<T, E>(arrival_time, target, earliest_possible_time);
    }

    return result;
};

template<typename T, typename E>
void reschedule_event(Time preferred_processing_time, std::weak_ptr<T> target, Time& earliest_possible_time) {
    earliest_possible_time = std::max(earliest_possible_time, preferred_processing_time);

    std::unique_ptr<Event> new_event = std::make_unique<E>(earliest_possible_time, target);
    Scheduler::get_instance().add(std::move(new_event));
};

bool SchedulingModule::notify_about_processing_finished(Time finish_time) {
    return notify_about_finish(finish_time, m_packet_cnt, m_earliest_possible_processing_time);
};

bool SchedulingModule::notify_about_arrival(Time arrival_time, std::weak_ptr<IProcessingDevice> target) {
    return notify_about_arriving<IProcessingDevice, Process>(arrival_time, target, m_packet_cnt, m_earliest_possible_processing_time);
};

bool SchedulingModule::notify_about_sending_finished(Time finish_time) {
    return notify_about_finish(finish_time, m_packet_to_send_cnt, m_earliest_possible_sending_time);
};

bool SchedulingModule::notify_about_new_packet_to_send(Time arrival_time, std::weak_ptr<ISender> target) {
    return notify_about_arriving<ISender, SendData>(arrival_time, target, m_packet_to_send_cnt, m_earliest_possible_sending_time);
};

}  // namespace sim
