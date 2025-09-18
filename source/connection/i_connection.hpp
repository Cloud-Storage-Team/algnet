#pragma once

#include <cstdint>
#include <memory>

#include "flow/i_flow.hpp"
#include "mplb/i_mplb.hpp"
#include "types.hpp"

namespace sim {

// Connection is a logical path between sender and receiver.
// It owns one or more flows and delegates flow selection to an MPLB.
// MPLB (Multi-Path Load Balancer) decides which flow to use for sending.
class IConnection : public Identifiable {
public:
    virtual ~IConnection() = default;

    virtual Id get_id() const = 0;
    // Start sending packets to flow-level
    virtual void start() = 0;
    // Add a flow to the connection
    virtual void add_flow(std::shared_ptr<IFlow> flow) = 0;
    // Delete a flow from the connection
    virtual void delete_flow(std::shared_ptr<IFlow> flow) = 0;
    // Adds more packets to the total amount to be sent
    virtual void add_data_to_send(SizeByte count_packets) = 0;
    // Called by a flow when an ACK is received to update connection state
    virtual void update(const std::shared_ptr<IFlow>& flow,
                        const FlowSample sample) = 0;
    virtual std::set<std::shared_ptr<IFlow>> get_flows() const = 0;
    virtual void clear_flows() = 0;
    virtual std::shared_ptr<IHost> get_sender() const = 0;
    virtual std::shared_ptr<IHost> get_receiver() const = 0;
};

}  // namespace sim
