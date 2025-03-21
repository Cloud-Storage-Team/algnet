#include <gtest/gtest.h>

#include "utils.hpp"
#include <random>

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

TEST_F(AddInlink, SameLinkMultipleTimes) {
    int NUMBER_OF_SOURCES = 5;
    int MAX_LINKS = 3;
    int NUMBER_OF_LOOPS = 3;

    auto sources = createRoutingModules(NUMBER_OF_SOURCES);
    auto dest = std::make_shared<sim::RoutingModule>(sim::RoutingModule());
    
    std::mt19937 gen(RANDOM_SEED);
    std::uniform_int_distribution<> dis(1, MAX_LINKS);

    std::unordered_map<std::shared_ptr<sim::ILink>, int> number_of_appearances;
    for (auto source: sources) {
        auto link = std::make_shared<TestLink>(source, dest);
        
        int links_to_add = dis(gen);
        for (size_t i = 0; i < links_to_add; ++i) {
            dest->add_inlink(link);
        }
        number_of_appearances[link] = 0;
    }

    for (size_t loop = 0; loop < NUMBER_OF_LOOPS; loop++) {
        for (size_t i = 0; i < NUMBER_OF_SOURCES; i++) {
            auto current_link = dest->next_inlink();
            number_of_appearances[current_link]++;
        }
    }

    for (auto appearances: number_of_appearances) {
        EXPECT_EQ(appearances.second, NUMBER_OF_LOOPS);
    }
}

}  // namespace test
