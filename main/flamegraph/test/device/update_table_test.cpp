#include <gtest/gtest.h>

#include "../utils/fake_packet.hpp"
#include "utils.hpp"

namespace test {

class UpdateTable : public testing::Test {
public:
    void TearDown() override {};
    void SetUp() override {};
};

TEST_F(UpdateTable, RouteIsPresent) {
    auto source = std::make_shared<TestDevice>("s1");
    auto neighbour = std::make_shared<TestDevice>("m1");
    auto dest = std::make_shared<TestDevice>("d1");
    auto another_dest = std::make_shared<TestDevice>("d2");

    auto link_neighbour =
        std::make_shared<TestLink>(TestLink(source, neighbour));

    EXPECT_EQ(source->get_link_to_destination(FakePacket(dest)), nullptr);
    EXPECT_EQ(source->get_link_to_destination(FakePacket(another_dest)),
              nullptr);

    source->update_routing_table(dest->get_id(), link_neighbour);

    EXPECT_EQ(source->get_link_to_destination(FakePacket(dest)),
              link_neighbour);
    EXPECT_EQ(source->get_link_to_destination(FakePacket(another_dest)),
              nullptr);
}

}  // namespace test
