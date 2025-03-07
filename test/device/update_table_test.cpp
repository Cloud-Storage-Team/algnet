#include <gtest/gtest.h>

#include "utils.hpp"

namespace test {

class UpdateTable : public testing::Test {
public:
    void TearDown() override{};
    void SetUp() override{};
};

TEST_F(UpdateTable, RouteIsPresent) {
    TestDevice source = TestDevice();
    TestDevice neighbour = TestDevice();
    TestDevice dest = TestDevice();
    TestDevice another_dest = TestDevice();

    TestLink link_neighbour = TestLink(&source, &neighbour);

    EXPECT_FALSE(source.check_route(&dest, &link_neighbour));
    EXPECT_FALSE(source.check_route(&another_dest, &link_neighbour));

    source.update_routing_table(&dest, &link_neighbour);

    EXPECT_TRUE(source.check_route(&dest, &link_neighbour));
    EXPECT_FALSE(source.check_route(&another_dest, &link_neighbour));
}

}  // namespace
