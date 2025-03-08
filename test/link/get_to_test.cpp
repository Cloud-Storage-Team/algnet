#include <gtest/gtest.h>

#include "utils.hpp"

namespace test {

TEST_F(LinkTest, SimpleTo) {
    std::shared_ptr<sim::IRoutingDevice> src =
        std::make_shared<DeviceMock>(DeviceMock());
    std::shared_ptr<sim::IRoutingDevice> dst =
        std::make_shared<DeviceMock>(DeviceMock());

    auto link = sim::Link(src, dst);
    ASSERT_EQ(link.get_to(), dst);
}

TEST_F(LinkTest, ToExpiredAfterCreation) {
    std::shared_ptr<sim::IRoutingDevice> src =
        std::make_shared<DeviceMock>(DeviceMock());
    std::shared_ptr<sim::IRoutingDevice> dst =
        std::make_shared<DeviceMock>(DeviceMock());
    auto link = sim::Link(src, dst);

    dst.reset();
    ASSERT_EQ(link.get_to(), nullptr);
}

TEST_F(LinkTest, ToExpiredBeforeCreation) {
    std::shared_ptr<sim::IRoutingDevice> src =
        std::make_shared<DeviceMock>(DeviceMock());
    std::shared_ptr<sim::IRoutingDevice> dst =
        std::make_shared<DeviceMock>(DeviceMock());
    dst.reset();

    auto link = sim::Link(src, dst);
    ASSERT_EQ(link.get_to(), nullptr);
}

}  // namespace test