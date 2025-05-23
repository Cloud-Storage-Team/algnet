#pragma once

#include <memory>
#include <set>

#include "device.hpp"

namespace sim {

class SchedulingModule : public ISchedulingModule {
public:
    ~SchedulingModule() = default;

    bool notify_about_processing_finished(Time finish_time) final;
    bool notify_about_arrival(Time arrival_time, std::weak_ptr<IProcessingDevice> target) final;

    bool notify_about_sending_finished(Time finish_time) final;
    bool notify_about_new_packet_to_send(Time arrival_time, std::weak_ptr<ISender> target) final;

private:
    std::uint32_t m_packet_cnt = 0;
    Time m_earliest_possible_processing_time = 0;

    std::uint32_t m_packet_to_send_cnt = 0;
    Time m_earliest_possible_sending_time = 0;
};

}  // namespace sim
