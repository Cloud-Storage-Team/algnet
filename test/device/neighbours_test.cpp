#include <gtest/gtest.h>

#include <algorithm>
#include <random>   

#include "utils.hpp"

namespace test {

class Neighbours : public testing::Test {
public:
    void TearDown() override {};
    void SetUp() override {};
};

std::vector<std::shared_ptr<TestLink>> create_random_links(
    const std::shared_ptr<sim::IRoutingDevice>& source,
    const std::vector<std::shared_ptr<sim::IRoutingDevice>>& dests,
    std::set<std::shared_ptr<sim::IRoutingDevice>>& target_neighbours,
    size_t number_of_links) {
    
    std::vector<std::shared_ptr<TestLink>> links;

    std::mt19937 gen(RANDOM_SEED);
    std::uniform_int_distribution<> dis(0, dests.size() - 1);

    for (size_t i = 0; i < number_of_links; ++i) {
        size_t random_index = dis(gen);
        auto random_target = dests[random_index];

        links.push_back(std::make_shared<TestLink>(TestLink(source, random_target)));
        target_neighbours.insert(random_target);
    }

    return links;
}

TEST_F(Neighbours, NeighboursAreCalculatedCorrectly) {
    size_t NUMBER_OF_DESTINATIONS = 5;
    auto source = std::make_shared<sim::RoutingModule>(sim::RoutingModule());
    auto dests = createRoutingModules(NUMBER_OF_DESTINATIONS);
    
    size_t NUMBER_OF_LINKS = 7;
    auto target_neighbours = std::set<std::shared_ptr<sim::IRoutingDevice>>();
    auto links = create_random_links(source, dests, target_neighbours, NUMBER_OF_LINKS);

    EXPECT_TRUE(source->get_neighbours().empty());
    for (size_t i = 0; i < NUMBER_OF_LINKS; i++) {
        source->update_routing_table(links[i]->get_to(), links[i]);
    }   
    
    std::vector<std::shared_ptr<sim::IRoutingDevice>> neighbours_vec =
        source->get_neighbours();
    std::set<std::shared_ptr<sim::IRoutingDevice>> neighbours(neighbours_vec.begin(), neighbours_vec.end());
    EXPECT_EQ(neighbours, target_neighbours);
}

}  // namespace test
