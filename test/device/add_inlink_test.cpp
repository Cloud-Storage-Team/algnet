#include <gtest/gtest.h>

#include "utils.hpp"

namespace test {

class AddInlink : public testing::Test {
public:
    void TearDown() override{};
    void SetUp() override{};
};

TEST_F(AddInlink, LinkIsPresent) {
    auto source = std::make_shared<sim::RoutingModule>(sim::RoutingModule());
    auto dest = std::make_shared<sim::RoutingModule>(sim::RoutingModule());
    auto link1 = std::make_shared<TestLink>(TestLink(source, dest));
    auto link2 = std::make_shared<TestLink>(TestLink(source, dest));

    EXPECT_FALSE(dest->has_inlink(link1));
    dest->add_inlink(link1);
    EXPECT_TRUE(dest->has_inlink(link1));
    EXPECT_FALSE(dest->has_inlink(link2));
    dest->add_inlink(link2);
    EXPECT_TRUE(dest->has_inlink(link1));
    EXPECT_TRUE(dest->has_inlink(link2));
}

}  // namespace test
