#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "packet.hpp"
#include "connection/mplb_policy.hpp"
#include "flow/i_flow.hpp"
#include "utils/identifier_factory.hpp"

namespace sim {

class Connection : public Identifiable,
                   public std::enable_shared_from_this<Connection> {
public:
    Connection(Id id, Id sender_id, Id receiver_id, std::shared_ptr<MPLBPolicy> policy);

    void add_flow(std::shared_ptr<IFlow> flow);

    void configure(std::uint64_t packet_count, std::size_t packet_size);

    void kick();                    
    void try_send_once();           
    void on_ack(const Packet& ack);

    Id get_id() const override;

private:
    bool send_via_flow(const std::shared_ptr<IFlow>& flow);

    Id                     m_id;
    Id                     m_sender_id;
    Id                     m_receiver_id;
    std::uint64_t          m_packets_left{0};
    std::size_t            m_packet_size{0};
    std::vector<std::shared_ptr<IFlow>> m_flows;
    std::shared_ptr<MPLBPolicy>         m_policy;
};

} // namespace sim
