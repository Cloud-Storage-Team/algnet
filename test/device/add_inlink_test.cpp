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
    auto link = std::make_shared<TestLink>(TestLink(source, dest));

    EXPECT_EQ(dest->next_inlink(), nullptr);
    dest->add_inlink(link);
    EXPECT_EQ(dest->next_inlink(), link);
}

}  // namespace test
