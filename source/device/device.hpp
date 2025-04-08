#pragma once

#include "types.hpp"

#include <memory>
#include <set>
#include <unordered_map>
#include <vector>

namespace sim {

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

class IRoutingDevice {
public:
    virtual ~IRoutingDevice() = default;

    virtual bool add_inlink(std::shared_ptr<ILink> link) = 0;
    virtual bool update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                                      std::shared_ptr<ILink> link) = 0;
    virtual std::vector<std::shared_ptr<IRoutingDevice>> get_neighbours()
        const = 0;
    virtual std::shared_ptr<ILink> get_link_to_destination(
        std::shared_ptr<IRoutingDevice> device) const = 0;
    virtual std::shared_ptr<ILink> next_inlink() = 0;
};

}  // namespace sim
