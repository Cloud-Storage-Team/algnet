#pragma once
#include <queue>

#include "device/routing_module.hpp"
#include "device/scheduling_module.hpp"
#include "ecn.hpp"
#include "event/process.hpp"
#include "event/send_data.hpp"
#include "interfaces/i_host.hpp"

namespace sim {

class Host : public IHost,
             public RoutingModule,
             public std::enable_shared_from_this<Host> {
public:
    Host(Id a_id, ECN a_ecn = ECN(1.0, 1.0, 1.0));
    ~Host() = default;

    bool notify_about_arrival(TimeNs arrive_time) final;

    TimeNs process() final;
    TimeNs send_packet() final;

    void enqueue_packet(Packet packet) final;

private:
    std::queue<Packet> m_nic_buffer;
    ECN m_ecn;
    SchedulingModule<IHost, Process> m_process_scheduler;
    SchedulingModule<IHost, SendData> m_send_data_scheduler;
};

}  // namespace sim
