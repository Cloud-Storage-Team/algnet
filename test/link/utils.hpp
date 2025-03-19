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

    virtual void add_inlink(std::shared_ptr<sim::ILink> link) final;
    virtual void update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                                      std::shared_ptr<sim::ILink> link) final;
    virtual std::shared_ptr<sim::ILink> next_inlink() final;
};

}  // namespace test