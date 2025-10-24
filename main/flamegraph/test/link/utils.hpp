#pragma once
#include <gtest/gtest.h>

#include <memory>

#include "device/interfaces/i_routing_device.hpp"
#include "link/link.hpp"
#include "packet.hpp"
#include "scheduler.hpp"

namespace test {

class LinkTest : public testing::Test {
public:
    void TearDown() override;
    void SetUp() override;
};

class DeviceMock : public sim::IDevice {
public:
    ~DeviceMock() = default;

    Id get_id() const final;
    bool add_inlink(std::shared_ptr<sim::ILink> link) final;
    bool add_outlink(std::shared_ptr<sim::ILink> link) final;
    bool update_routing_table(Id dest_id, std::shared_ptr<sim::ILink> link,
                              size_t paths_count) final;
    std::shared_ptr<sim::ILink> next_inlink() final;
    std::shared_ptr<sim::ILink> get_link_to_destination(
        sim::Packet packet) const final;
    std::set<std::shared_ptr<sim::ILink>> get_outlinks() final;
    bool notify_about_arrival(TimeNs arrival_time) final;

    TimeNs process() final;
};

}  // namespace test
