#pragma once

#include <cstdint>
#include <functional>
#include <memory>

#include "connection/flow/i_flow.hpp"
#include "data.hpp"
#include "mplb/i_mplb.hpp"
#include "utils/str_expected.hpp"

namespace sim {

// Connection is a logical path between sender and receiver.
// It owns one or more flows and delegates flow selection to an MPLB.
// MPLB (Multi-Path Load Balancer) decides which flow to use for sending.
class IConnection : public Identifiable {
public:
    virtual ~IConnection() = default;

    virtual Id get_id() const = 0;
    // Add a flow to the connection
    virtual void add_flow(std::shared_ptr<IFlow> flow) = 0;
    // Delete a flow from the connection
    virtual void delete_flow(std::shared_ptr<IFlow> flow) = 0;

    using OnDeliveryCallback = std::function<void()>;

    // Adds new portion of data that should be sent
    // Callback hould be triggered when given data succesefully delivered
    virtual utils::StrExpected<void> add_data_to_send(
        Data data, OnDeliveryCallback callback = []() {}) = 0;
    // Returns the total amount of data added to the connection
    virtual SizeByte get_total_data_added() const = 0;
    // Called by a flow when an ACK is received to update connection state
    virtual void update(const std::shared_ptr<IFlow>& flow) = 0;
    virtual std::set<std::shared_ptr<IFlow>> get_flows() const = 0;
    virtual void clear_flows() = 0;
    virtual std::shared_ptr<IHost> get_sender() const = 0;
    virtual std::shared_ptr<IHost> get_receiver() const = 0;
};

}  // namespace sim
