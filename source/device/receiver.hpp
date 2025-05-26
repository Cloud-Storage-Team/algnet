#pragma once
#include <memory>

#include "packet.hpp"
#include "routing_module.hpp"
#include "utils/identifier_factory.hpp"

namespace sim {

struct Packet;

class IReceiver : public IRoutingDevice,
                  public IProcessingDevice,
                  public Identifiable {
public:
    virtual ~IReceiver() = default;
};

class Receiver : public IReceiver,
                 public std::enable_shared_from_this<Receiver> {
public:
    Receiver(Id a_id);
    ~Receiver() = default;

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
    // Process a packet by removing it from the ingress buffer
    // Send an ACK to the egress buffer
    // and schedule next receive event after a delay.
    // Upon receiving send an ACK to the sender.
    // Packets are taken from ingress buffers on a round-robin basis.
    // The iterator over ingress buffers is stored in m_next_link.
    Time process() final;

    Id get_id() const final;

private:
    Time send_ack(Packet data_packet);
    std::unique_ptr<RoutingModule> m_router;
    Id m_id;
};

}  // namespace sim
