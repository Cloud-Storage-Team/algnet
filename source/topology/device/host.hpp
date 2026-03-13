#pragma once
#include <queue>

#include "ecn.hpp"
#include "interfaces/i_host.hpp"
#include "scheduler/event/process.hpp"
#include "scheduler/event/send_data.hpp"
#include "topology/device/routing_module.hpp"
#include "topology/device/scheduling_module.hpp"

namespace sim {

class Host : public IHost,
             public RoutingModule,
             public std::enable_shared_from_this<Host> {
public:
    Host(Id a_id, ECN a_ecn = ECN(1.0, 1.0, 1.0));
    ~Host() = default;

    bool notify_about_arrival() final;

    TimeNs process() final;
    TimeNs send_packet() final;

    void enqueue_packet(const Packet& packet) final;

private:
    std::queue<Packet> m_nic_buffer;
    ECN m_ecn;
    SchedulingModule<IHost, Process> m_process_scheduler;
    SchedulingModule<IHost, SendData> m_send_data_scheduler;
};

}  // namespace sim
