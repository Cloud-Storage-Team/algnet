#include <gtest/gtest.h>

#include "utils.hpp"

namespace test {

TEST_F(LinkTest, NoPacketToGet) {
    std::shared_ptr<sim::IDevice> src =
        std::make_shared<DeviceMock>(DeviceMock());
    std::shared_ptr<sim::IDevice> dst =
        std::make_shared<DeviceMock>(DeviceMock());
    auto link = std::make_shared<sim::Link>("", src, dst);

    ASSERT_FALSE(link->get_packet().has_value());
}

}  // namespace test
