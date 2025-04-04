#pragma once
#include "device.hpp"
#include "link.hpp"

namespace test {

class ReceiverMock : public sim::IReceiver {
public:
    ReceiverMock() = default;
    ~ReceiverMock() = default;

    bool add_inlink(std::shared_ptr<sim::ILink> link) final;
    bool update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                              std::shared_ptr<sim::ILink> link) final;
    std::vector<std::shared_ptr<IRoutingDevice>> get_neighbours() const final;
    std::shared_ptr<sim::ILink> next_inlink() final;
    std::shared_ptr<sim::ILink> get_link_to_destination(
        std::shared_ptr<IRoutingDevice> device) const final;

    void process() final;
    sim::DeviceType get_type() const final;
};

}  // namespace test
