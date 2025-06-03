#pragma once

#include <memory>
#include <set>

#include "types.hpp"
#include "packet.hpp"
#include "interfaces/link_interface.hpp"
#include "utils/identifier_factory.hpp"

namespace sim {

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

class IRoutingDevice : public Identifiable {
public:
    virtual ~IRoutingDevice() = default;

    virtual bool add_inlink(std::shared_ptr<ILink> link) = 0;
    virtual bool add_outlink(std::shared_ptr<ILink> link) = 0;
    virtual bool update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                                      std::shared_ptr<ILink> link,
                                      size_t paths_count = 1) = 0;
    virtual std::shared_ptr<ILink> get_link_to_destination(
        std::shared_ptr<IRoutingDevice> device) const = 0;
    virtual std::shared_ptr<ILink> next_inlink() = 0;
    virtual std::set<std::shared_ptr<ILink>> get_outlinks() = 0;
};

class IReceiver : public IRoutingDevice, public IProcessingDevice {
public:
    virtual ~IReceiver() = default;
};

class ISender : public IRoutingDevice, public IProcessingDevice {
public:
    virtual ~ISender() = default;
    virtual void enqueue_packet(Packet packet) = 0;
    virtual Time send_data() = 0;
};

class ISwitch : public IRoutingDevice, public IProcessingDevice {
public:
    virtual ~ISwitch() = default;
};

}  // namespace sim
