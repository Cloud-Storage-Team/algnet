#include "scheduling_module.hpp"

#include <algorithm>
#include <memory>

#include "event.hpp"
#include "scheduler.hpp"
#include "logger/logger.hpp"

namespace sim {

bool SchedulingModule::notify_about_processing_finished(Time finish_time) {
    if (m_packet_cnt == 0) {
        // TODO warning, impossible situation
        return false;
    }
    m_packet_cnt--;
    m_earliest_possible_processing_time = finish_time;

    return (m_packet_cnt == 0);
};

bool SchedulingModule::notify_about_arrival(Time arrival_time, std::weak_ptr<IProcessingDevice> target) {
    m_packet_cnt++;
    bool result = (m_packet_cnt == 1);
    if (result) {
        reschedule_process(arrival_time, target);
    }

    return result;
};

void SchedulingModule::reschedule_process(Time preferred_processing_time, std::weak_ptr<IProcessingDevice> target) {
    m_earliest_possible_processing_time = std::max(m_earliest_possible_processing_time, preferred_processing_time);

    std::unique_ptr<Event> new_process_event = std::make_unique<Process>(m_earliest_possible_processing_time, target);
    Scheduler::get_instance().add(std::move(new_process_event));
};

}  // namespace sim
