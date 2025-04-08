#include <gtest/gtest.h>

#include <random>

#include "utils.hpp"

namespace test {

class AddLink : public testing::Test {
public:
    void TearDown() override {};
    void SetUp() override {};
};

TEST_F(AddLink, InlinkIsPresent) {
    auto source = std::make_shared<sim::RoutingModule>(sim::RoutingModule());
    auto dest = std::make_shared<sim::RoutingModule>(sim::RoutingModule());
    auto link = std::make_shared<TestLink>(TestLink(source, dest));

    EXPECT_EQ(dest->next_inlink(), nullptr);
    dest->add_inlink(link);
    EXPECT_EQ(dest->next_inlink(), link);
}

TEST_F(AddLink, SameInlinkMultipleTimes) {
    int NUMBER_OF_SOURCES = 5;
    int MAX_LINKS = 3;
    int NUMBER_OF_LOOPS = 3;

    auto sources = createRoutingModules(NUMBER_OF_SOURCES);
    auto dest = std::make_shared<sim::RoutingModule>(sim::RoutingModule());

    std::mt19937 gen(RANDOM_SEED);
    std::uniform_int_distribution<> dis(1, MAX_LINKS);

    std::unordered_map<std::shared_ptr<sim::ILink>, int> number_of_appearances;
    for (auto source : sources) {
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

    for (auto appearances : number_of_appearances) {
        EXPECT_EQ(appearances.second, NUMBER_OF_LOOPS);
    }
}

TEST_F(AddLink, OutlinkIsPresent) {
    auto source = std::make_shared<sim::RoutingModule>(sim::RoutingModule());
    auto dest = std::make_shared<sim::RoutingModule>(sim::RoutingModule());
    auto link = std::make_shared<TestLink>(TestLink(source, dest));

    EXPECT_TRUE(source->get_outlinks().empty());
    source->add_outlink(link);
    auto outlinks = source->get_outlinks();
    EXPECT_TRUE(outlinks.contains(link));
    EXPECT_EQ(*outlinks.begin(), link);
}

TEST_F(AddLink, SameOutlinkMultipleTimes) {
    int NUMBER_OF_DESTINATIONS = 5;
    int MAX_LINKS = 3;
    int NUMBER_OF_LOOPS = 3;

    auto destinations = createRoutingModules(NUMBER_OF_DESTINATIONS);
    auto source = std::make_shared<sim::RoutingModule>(sim::RoutingModule());

    std::mt19937 gen(RANDOM_SEED);
    std::uniform_int_distribution<> dis(1, MAX_LINKS);

    std::unordered_map<std::shared_ptr<sim::ILink>, int> number_of_appearances;
    for (auto dest : destinations) {
        auto link = std::make_shared<TestLink>(source, dest);

        int links_to_add = dis(gen);
        for (size_t i = 0; i < links_to_add; ++i) {
            source->add_outlink(link);
        }
        number_of_appearances[link] = 0;
    }

    auto outlinks = source->get_outlinks();
    auto next_outlink = outlinks.begin();
    for (size_t loop = 0; loop < NUMBER_OF_LOOPS; loop++) {
        for (size_t i = 0; i < NUMBER_OF_DESTINATIONS; i++) {
            auto current_link = *next_outlink;
            number_of_appearances[current_link]++;
            next_outlink++;
            if (next_outlink == outlinks.end()) {
                next_outlink = outlinks.begin();
            }
        }
    }

    for (auto appearances : number_of_appearances) {
        EXPECT_EQ(appearances.second, NUMBER_OF_LOOPS);
    }
}

}  // namespace test
