#pragma once
#include <gtest/gtest.h>

#include <memory>

#include "device/device.hpp"
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

    bool add_inlink(std::shared_ptr<sim::ILink> link) final;
    bool add_outlink(std::shared_ptr<sim::ILink> link) final;
    bool update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                              std::shared_ptr<sim::ILink> link, size_t paths_count) final;
    std::shared_ptr<sim::ILink> next_inlink() final;
    std::shared_ptr<sim::ILink> get_link_to_destination(
        std::shared_ptr<IRoutingDevice> dest) const final;
    std::set<std::shared_ptr<sim::ILink>> get_outlinks() final;
};

}  // namespace test
