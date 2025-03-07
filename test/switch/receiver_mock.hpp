#pragma once
#include "../../source/device.hpp"
#include "../../source/link.hpp"

namespace test {

class ReceiverMock : public sim::IReceiver {
public:
    ReceiverMock() = default;
    ~ReceiverMock() = default;

    void add_inlink(std::shared_ptr<sim::Link> link) final;
    void update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                              std::shared_ptr<sim::Link> link) final;

    std::shared_ptr<sim::Link> next_inlink() final;
    std::shared_ptr<sim::Link> get_destination(
        std::shared_ptr<IRoutingDevice> dest) const final;

    void process() final;
    sim::DeviceType get_type() const final;
};

}  // namespace test
