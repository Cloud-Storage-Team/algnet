#include <gtest/gtest.h>

#include "utils.hpp"

namespace test {

class AddInlink : public testing::Test {
public:
    void TearDown() override{};
    void SetUp() override{};
};

TEST_F(AddInlink, LinkIsPresent) {
   TestDevice source = TestDevice();
   TestDevice dest = TestDevice();
   TestLink link1 = TestLink(&source, &dest);
   TestLink link2 = TestLink(&source, &dest);

    EXPECT_FALSE(dest.has_inlink(&link1));
    dest.add_inlink(&link1);
    EXPECT_TRUE(dest.has_inlink(&link1));
    EXPECT_FALSE(dest.has_inlink(&link2));
    dest.add_inlink(&link2);
    EXPECT_TRUE(dest.has_inlink(&link1));
    EXPECT_TRUE(dest.has_inlink(&link2));
}

}  // namespace
