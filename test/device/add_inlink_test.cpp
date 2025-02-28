#include <gtest/gtest.h>

#include "utils.hpp"

namespace {

class AddInlink : public testing::Test {
public:
    void TearDown() override{};
    void SetUp() override{};
};

TEST_F(AddInlink, LinkPresents) {
    sim::TestDevice* source = new sim::TestDevice();
    sim::TestDevice* dest = new sim::TestDevice();
    sim::TestLink* link1 = new sim::TestLink(source, dest);
    sim::TestLink* link2 = new sim::TestLink(source, dest);

    EXPECT_FALSE(dest->has_inlink(link1));
    dest->add_inlink(link1);
    EXPECT_TRUE(dest->has_inlink(link1));
    EXPECT_FALSE(dest->has_inlink(link2));
    dest->add_inlink(link2);
    EXPECT_TRUE(dest->has_inlink(link1));
    EXPECT_TRUE(dest->has_inlink(link2));
}

}  // namespace
