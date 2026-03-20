#pragma once
#include <queue>

#include "ecn.hpp"
#include "interfaces/i_host.hpp"
#include "routing_module/routing_module.hpp"

namespace sim {

class Host : public IHost,
             public RoutingModule,
             public std::enable_shared_from_this<Host> {
public:
    Host(Id a_id, ECN a_ecn = ECN(1.0, 1.0, 1.0));
    ~Host() = default;

    bool notify_about_arrival() final;

    void enqueue_packet(const Packet& packet) final;

private:
    void process();
    void send_packet();

    std::queue<Packet> m_nic_buffer;
    ECN m_ecn;

    std::size_t m_packets_on_inlinks = 0;
};

}  // namespace sim
