#pragma once
#include <queue>

#include "device/routing_module.hpp"
#include "device/scheduling_module.hpp"
#include "event.hpp"
#include "interfaces/i_host.hpp"

namespace sim {

class Host : public IHost, public std::enable_shared_from_this<Host> {
public:
    Host(Id id);
    ~Host() = default;

    bool add_inlink(std::shared_ptr<ILink> link) final;
    bool add_outlink(std::shared_ptr<ILink> link) final;
    bool update_routing_table(Id dest_id, std::shared_ptr<ILink> link,
                              size_t paths_count = 1) final;
    std::shared_ptr<ILink> next_inlink() final;
    std::shared_ptr<ILink> get_link_to_destination(Packet packet) const final;
    std::set<std::shared_ptr<ILink>> get_outlinks() final;
    bool notify_about_arrival(Time arrive_time) final;

    DeviceType get_type() const final;
    Time process() final;
    Time send_packet() final;

    void enqueue_packet(Packet packet) final;

    Id get_id() const final;

private:
    std::queue<Packet> m_flow_buffer;
    RoutingModule m_router;
    SchedulingModule<IHost, Process> m_process_scheduler;
    SchedulingModule<IHost, SendData> m_send_data_scheduler;
};

}  // namespace sim
