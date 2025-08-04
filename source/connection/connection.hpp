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
    Connection(Id a_id, Id a_sender_id, Id a_receiver_id, SizeByte a_packet_size, std::shared_ptr<MPLBPolicy> policy, std::uint64_t a_packet_count = 0);

    void add_flow(std::shared_ptr<IFlow> flow);

    void configure(std::uint64_t packet_count);

    void kick();                    
    void try_send_once();           
    void on_ack(const Packet& ack);

    Id get_id() const override;
    Id get_sender_id() const { return m_sender_id; }
    Id get_receiver_id() const { return m_receiver_id; }
    SizeByte get_packet_size() const { return m_packet_size; }

private:
    bool send_via_flow(const std::shared_ptr<IFlow>& flow);

    Id                     m_id;
    Id                     m_sender_id;
    Id                     m_receiver_id;
    SizeByte               m_packet_size;
    std::shared_ptr<MPLBPolicy>         m_policy;
    std::uint64_t          m_packets_left;
    std::vector<std::shared_ptr<IFlow>> m_flows;
};

} // namespace sim
