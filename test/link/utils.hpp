#pragma once
#include <gtest/gtest.h>

#include "device.hpp"
#include "link.hpp"

namespace test {

class LinkTest : public testing::Test {
public:
    void TearDown() override;
    void SetUp() override;
};

class DeviceMock : public sim::IRoutingDevice {
public:
    ~DeviceMock() = default;

    virtual bool add_inlink(std::shared_ptr<sim::ILink> link) final;
    virtual bool update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                                      std::shared_ptr<sim::ILink> link) final;
    virtual std::shared_ptr<sim::ILink> next_inlink() final;
    virtual std::vector<std::shared_ptr<IRoutingDevice>> get_neighbours()
        const final;
    virtual std::shared_ptr<sim::ILink> get_link_to_destination(
        std::shared_ptr<IRoutingDevice> device) const final;
};

}  // namespace test
