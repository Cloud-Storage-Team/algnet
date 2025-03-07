#include <gtest/gtest.h>

#include "utils.hpp"

namespace test {

class LinkToDevice : public testing::Test {
public:
    void TearDown() override{};
    void SetUp() override{};
};

TEST_F(LinkToDevice, NoLinkToDevice) {
    TestDevice source = TestDevice();
    TestDevice dest = TestDevice();

    EXPECT_EQ(source.get_link_to_device(&dest), nullptr);
}

TEST_F(LinkToDevice, LinkIsPresent) {
    TestDevice source = TestDevice();
    TestDevice neighbour = TestDevice();
    TestDevice dest = TestDevice();
    TestDevice another_dest = TestDevice();

    TestLink link_neighbour = TestLink(&source, &neighbour);
    source.update_routing_table(&neighbour, &link_neighbour);
    TestLink link_dest = TestLink(&source, &dest);
    source.update_routing_table(&dest, &link_dest);

    EXPECT_EQ(source.get_link_to_device(&dest), link_dest);
    EXPECT_EQ(source.get_link_to_device(&neighbour), link_neighbour);
    EXPECT_EQ(source.get_link_to_device(&another_dest), nullptr);
    source.update_routing_table(&another_dest, &link_neighbour);

    EXPECT_EQ(source.get_link_to_device(&another_dest), link_neighbour);
}

}  // namespace
