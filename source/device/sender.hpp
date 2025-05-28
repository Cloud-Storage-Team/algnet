#pragma once
#include <memory>
#include <queue>

#include "device.hpp"
#include "routing_module.hpp"
#include "utils/identifier_factory.hpp"

namespace sim {

struct Packet;

class ISender : public IRoutingDevice, public IProcessingDevice {
public:
    virtual ~ISender() = default;
    virtual void enqueue_packet(Packet packet) = 0;
    virtual Time send_data() = 0;
};

class Sender : public ISender,
               public std::enable_shared_from_this<Sender> {
public:
    Sender(Id a_id);
    ~Sender() = default;

    bool add_inlink(std::shared_ptr<ILink> link) final;
    bool add_outlink(std::shared_ptr<ILink> link) final;
    bool update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                              std::shared_ptr<ILink> link,
                              size_t paths_count = 1) final;
    std::shared_ptr<ILink> next_inlink() final;
    std::shared_ptr<ILink> get_link_to_destination(
        std::shared_ptr<IRoutingDevice> dest) const final;
    std::set<std::shared_ptr<ILink>> get_outlinks() final;

    DeviceType get_type() const final;
    // Process an ACK by removing it from the ingress buffer,
    // update the flow state,
    // and schedule next ACK processing event after a delay.
    // Send the next packet: dequeue from the flow buffer
    // and move it to the egress port (link)
    // ACKs are taken from ingress buffers on a round-robin basis.
    // The iterator over ingress buffers is stored in m_next_link.
    Time process() final;
    Time send_data() final;

    void enqueue_packet(Packet packet) final;

    Id get_id() const final;

private:
    std::queue<Packet> m_flow_buffer;
    std::unique_ptr<RoutingModule> m_router;
};

}  // namespace sim
