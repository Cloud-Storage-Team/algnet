#include <gtest/gtest.h>

#include "utils.hpp"

namespace test {

TEST_F(LinkTest, NoPacketToGet) {
    std::shared_ptr<sim::IDevice> src =
        std::make_shared<DeviceMock>(DeviceMock());
    std::shared_ptr<sim::IDevice> dst =
        std::make_shared<DeviceMock>(DeviceMock());
    auto link = sim::Link::create_shared("", src, dst);

    ASSERT_FALSE(link->has_packet());
}

}  // namespace test
