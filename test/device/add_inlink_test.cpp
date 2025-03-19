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
    int NUMBER_OF_LINKS = 2;
    int NUMBER_OF_LOOPS = 3;
    auto sources = createRoutingModules(NUMBER_OF_LINKS);
    auto dest = std::make_shared<sim::RoutingModule>(sim::RoutingModule());

    auto links = std::vector<std::shared_ptr<TestLink>>();
    for (int i = 0; i < NUMBER_OF_LINKS; i++) {
        links.emplace_back(
            std::make_shared<TestLink>(TestLink(sources[i], dest)));
    }

    EXPECT_EQ(dest->next_inlink(), nullptr);
    for (int i = 0; i < NUMBER_OF_LINKS; i++) {
        dest->add_inlink(links[i]);
    }
    dest->add_inlink(links[0]);

    auto first_link = dest->next_inlink();
    EXPECT_NE(first_link, nullptr);
    for (int i = 1; i < NUMBER_OF_LINKS * NUMBER_OF_LOOPS; i++) {
        if (i % NUMBER_OF_LINKS == 0) {
            EXPECT_EQ(links[i % NUMBER_OF_LINKS], first_link);
        }
    }
}

}  // namespace test
