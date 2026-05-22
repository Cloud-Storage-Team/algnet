#pragma once

#include <set>

#include "network/connection/flow/packet.hpp"
#include "utils/identifiable.hpp"

namespace sim {

class ILink;

class IRoutingDevice : public Identifiable {
public:
    virtual ~IRoutingDevice() = default;

    virtual bool add_inlink(std::shared_ptr<ILink> link) = 0;
    virtual bool add_outlink(std::shared_ptr<ILink> link) = 0;
    virtual bool update_routing_table(Id dest_id, std::shared_ptr<ILink> link,
                                      size_t paths_count = 1) = 0;
    virtual std::shared_ptr<ILink> get_link_to_destination(
        PacketPtr packet) const = 0;
    virtual std::shared_ptr<ILink> next_inlink() = 0;
    virtual std::set<std::shared_ptr<ILink>> get_outlinks() = 0;
};

}  // namespace sim
