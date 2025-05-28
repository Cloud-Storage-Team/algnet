#pragma once

#include <memory>
#include <set>

#include "types.hpp"

namespace sim {

struct Packet;
class ILink;

enum DeviceType { SWITCH, SENDER, RECEIVER };

class IProcessingDevice {
public:
    virtual ~IProcessingDevice() = default;

    // One step of device work cycle;
    // e.g. see next inlink, take one packet from it,
    // and do smth with it (send further, send ask etc)
    virtual Time process() = 0;

    virtual DeviceType get_type() const = 0;
};

class IRoutingDevice: public Identifiable {
public:
    virtual ~IRoutingDevice() = default;

    virtual bool add_inlink(std::shared_ptr<ILink> link) = 0;
    virtual bool add_outlink(std::shared_ptr<ILink> link) = 0;
    virtual bool update_routing_table(Id dest_id, std::shared_ptr<ILink> link, size_t paths_count = 1) = 0;
    virtual std::shared_ptr<ILink> get_link_to_destination(Packet packet) const = 0;
    virtual std::shared_ptr<ILink> next_inlink() = 0;
    virtual std::set<std::shared_ptr<ILink>>get_outlinks() = 0;
};

}  // namespace sim
