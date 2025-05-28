#pragma once

#include <memory>
#include <set>

#include "types.hpp"
#include "utils/identifier_factory.hpp"

namespace sim {

class ILink;
class ISender;

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
    virtual std::set<std::shared_ptr<ILink>>get_outlinks() = 0;
    
    // Returns true if the total number of packets in inlinks change from 0 to 1 
    virtual bool notify_about_arrival(Time arrival_time) = 0;
};

class ISchedulingModule {
public:
    virtual ~ISchedulingModule() = default;

    virtual bool notify_about_processing_finished(Time finish_time) = 0;
    // Returns true if the total number of packets in inlinks change from 0 to 1 
    virtual bool notify_about_arrival(Time arrival_time, std::weak_ptr<IProcessingDevice> target) = 0;
    
    virtual bool notify_about_sending_finished(Time finish_time) = 0;
    // Returns true if the total number of packets in NIC buffer change from 0 to 1 
    virtual bool notify_about_new_packet_to_send(Time arrival_time, std::weak_ptr<ISender> target) = 0;
};

}  // namespace sim
