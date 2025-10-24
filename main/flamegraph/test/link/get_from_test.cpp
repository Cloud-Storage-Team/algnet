#include <gtest/gtest.h>

#include "utils.hpp"

namespace test {

TEST_F(LinkTest, SimpleFrom) {
    std::shared_ptr<sim::IDevice> src =
        std::make_shared<DeviceMock>(DeviceMock());
    std::shared_ptr<sim::IDevice> dst =
        std::make_shared<DeviceMock>(DeviceMock());

    auto link = std::make_shared<sim::Link>("", src, dst);
    ASSERT_EQ(link->get_from(), src);
}

TEST_F(LinkTest, FromExpiredAfterCreation) {
    std::shared_ptr<sim::IDevice> src =
        std::make_shared<DeviceMock>(DeviceMock());
    std::shared_ptr<sim::IDevice> dst =
        std::make_shared<DeviceMock>(DeviceMock());
    auto link = std::make_shared<sim::Link>("", src, dst);

    src.reset();
    ASSERT_EQ(link->get_from(), nullptr);
}

TEST_F(LinkTest, FromExpiredBeforeCreation) {
    std::shared_ptr<sim::IDevice> src =
        std::make_shared<DeviceMock>(DeviceMock());
    std::shared_ptr<sim::IDevice> dst =
        std::make_shared<DeviceMock>(DeviceMock());
    src.reset();

    auto link = std::make_shared<sim::Link>("", src, dst);
    ASSERT_EQ(link->get_from(), nullptr);
}

}  // namespace test
