#include <gtest/gtest.h>

#include "utils.hpp"

namespace test {

class AddInlink : public testing::Test {
public:
    void TearDown() override {};
    void SetUp() override {};
};

TEST_F(AddInlink, LinkIsPresent) {
    auto source = std::make_shared<sim::RoutingModule>(sim::RoutingModule());
    auto dest = std::make_shared<sim::RoutingModule>(sim::RoutingModule());
    auto link = std::make_shared<TestLink>(TestLink(source, dest));

    EXPECT_EQ(dest->next_inlink(), nullptr);
    dest->add_inlink(link);
    EXPECT_EQ(dest->next_inlink(), link);
}

TEST_F(AddInlink, SameLinkTwice) {
    int NUMBER_OF_ITERATIONS = 6;
    int FIRST_LINK_VALUE = 1;
    int SECOND_LINK_VALUE = 10;
    int EXPECTED_RESULT =
        (FIRST_LINK_VALUE + SECOND_LINK_VALUE) * (NUMBER_OF_ITERATIONS / 2);

    auto source = std::make_shared<sim::RoutingModule>(sim::RoutingModule());
    auto doubled_source =
        std::make_shared<sim::RoutingModule>(sim::RoutingModule());
    auto dest = std::make_shared<sim::RoutingModule>(sim::RoutingModule());

    auto link = std::make_shared<TestLink>(
        TestLink(source, dest, sim::Packet(sim::DATA, 1)));
    auto doubled_link = std::make_shared<TestLink>(
        TestLink(source, dest, sim::Packet(sim::DATA, 10)));

    dest->add_inlink(doubled_link);
    dest->add_inlink(link);
    dest->add_inlink(doubled_link);

    int sum = 0;
    for (int i = 0; i < NUMBER_OF_ITERATIONS; i++) {
        sum += dest->next_inlink()->get_packet()->size;
    }
    EXPECT_EQ(sum, EXPECTED_RESULT);
}

}  // namespace test
