#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "connection/i_mplb.hpp"
#include "flow/i_flow.hpp"
#include "packet.hpp"
#include "utils/identifier_factory.hpp"

namespace sim {

class Connection : public Identifiable,
                   public std::enable_shared_from_this<Connection> {
public:
    Connection(Id a_id, Id a_sender_id, Id a_receiver_id,
               SizeByte a_packet_size, std::shared_ptr<IMPLB> mplb,
               std::uint64_t a_num_packets_to_send = 0);

    Id get_id() const final;

    // Start sending packets to flow-level
    void start();

    // Add a flow to the connection
    void add_flow(std::shared_ptr<IFlow> flow);

    // Delete a flow from the connection
    void delete_flow(std::shared_ptr<IFlow> flow);

    // Adds more packets to the total amount to be sent
    void add_packets_to_send(std::uint64_t count_packets);

    // Called by a flow when an ACK is received to update connection state
    void update(std::shared_ptr<IFlow> flow, const FlowSample sample);

    // Returns true if the connection has any registered flows
    bool has_flows() const;

    void clear_flows();

private:
    // Tries to send packets using the MPLB-selected flow(s), as long as allowed.
    void send_packets();

    Id m_id;
    Id m_sender_id;
    Id m_receiver_id;
    SizeByte m_packet_size;
    std::shared_ptr<IMPLB> m_mplb;
    std::uint64_t m_packets_to_send;
    std::set<std::shared_ptr<IFlow>> m_flows;
};

}  // namespace sim
