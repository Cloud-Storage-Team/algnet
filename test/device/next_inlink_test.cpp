#include <gtest/gtest.h>

#include "utils.hpp"

namespace test {

class LinkToDevice : public testing::Test {
public:
    void TearDown() override {};
    void SetUp() override {};
};

TEST_F(LinkToDevice, RoundRobin) {
    int NUMBER_OF_LINKS = 2;
    int NUMBER_OF_LOOPS = 3;
    auto sources = createRoutingModules(NUMBER_OF_LINKS);
    auto dest = std::make_shared<sim::RoutingModule>(sim::RoutingModule());

    auto links = std::vector<std::shared_ptr<TestLink>>();
    for (int i = 0; i < NUMBER_OF_LINKS; i++) {
        links.emplace_back(
            std::make_shared<TestLink>(TestLink(sources[i], dest)));
    }

    EXPECT_EQ(dest->next_inlink().lock(), nullptr);
    for (int i = 0; i < NUMBER_OF_LINKS; i++) {
        dest->add_inlink(links[i]);
    }

    std::vector<std::shared_ptr<sim::ILink>> first_inlinks_loop;
    for (int i = 0; i < NUMBER_OF_LINKS; i++) {
        first_inlinks_loop.emplace_back(dest->next_inlink());
    }

    for (int i = 0; i < NUMBER_OF_LOOPS; i++) {
        std::vector<std::shared_ptr<sim::ILink>> new_inlinks_loop;
        for (int j = 0; j < NUMBER_OF_LINKS; j++) {
            new_inlinks_loop.emplace_back(dest->next_inlink());
        }

        ASSERT_EQ(first_inlinks_loop, new_inlinks_loop);
    }
}

}  // namespace test
