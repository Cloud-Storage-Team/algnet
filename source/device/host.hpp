#pragma once
#include <queue>

#include "device/routing_module.hpp"
#include "device/scheduling_module.hpp"
#include "event/process.hpp"
#include "event/send_data.hpp"
#include "interfaces/i_host.hpp"

namespace sim {

class Host : public IHost, public RoutingModule, public std::enable_shared_from_this<Host> {
public:
    Host(Id id);
    ~Host() = default;

    bool notify_about_arrival(Time arrive_time) final;

    DeviceType get_type() const final;
    Time process() final;
    Time send_packet() final;

    void enqueue_packet(Packet packet) final;

private:
    std::queue<Packet> m_nic_buffer;
    SchedulingModule<IHost, Process> m_process_scheduler;
    SchedulingModule<IHost, SendData> m_send_data_scheduler;
};

}  // namespace sim
