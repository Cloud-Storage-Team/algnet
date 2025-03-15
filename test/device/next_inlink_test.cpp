#include <gtest/gtest.h>

#include "utils.hpp"

namespace test {

class LinkToDevice : public testing::Test {
public:
    void TearDown() override{};
    void SetUp() override{};
};

TEST_F(LinkToDevice, RoundRobin) {
    auto source = std::make_shared<sim::RoutingModule>(sim::RoutingModule());
    auto dest = std::make_shared<sim::RoutingModule>(sim::RoutingModule());
    int NUMBER_OF_LINKS = 5;

    for (int i = 0; i < NUMBER_OF_LINKS; i++) {
        dest->add_inlink(std::make_shared<TestLink>(TestLink(source, dest, sim::Packet(sim::DATA, i))));
    }

    std::set<std::uint32_t> sizes{};
    for (int i = 0; i < NUMBER_OF_LINKS; i++) {
        sizes.insert(dest->next_inlink()->get_packet().value().size);
    }

    EXPECT_EQ(sizes.size(), NUMBER_OF_LINKS);
    int size = 0;
    for (auto num: sizes) {
        EXPECT_EQ(num, size++);
    }
}

}  // namespace test
