#include <gtest/gtest.h>

#include <memory>
#include <random>

#include "device/device.hpp"
#include "link.hpp"
#include "logger/logger.hpp"
#include "utils.hpp"
#include "utils/loop_iterator.hpp"

namespace test {

class AddLink : public testing::Test {
public:
    void TearDown() override {};
    void SetUp() override {};
};

TEST_F(AddLink, LinkIsPresent) {
    auto source = std::make_shared<sim::RoutingModule>(sim::RoutingModule());
    auto dest = std::make_shared<sim::RoutingModule>(sim::RoutingModule());
    auto link = std::make_shared<TestLink>(TestLink(source, dest));

    EXPECT_EQ(dest->next_inlink(), nullptr);
    dest->add_inlink(link);
    EXPECT_EQ(dest->next_inlink(), link);

    EXPECT_TRUE(source->get_outlinks().empty());
    source->add_outlink(link);
    auto outlinks = source->get_outlinks();
    EXPECT_TRUE(outlinks.contains(link));
    EXPECT_EQ((*outlinks.begin()).lock(), link);
}

TEST_F(AddLink, SameLinkMultipleTimes) {
    size_t NUMBER_OF_NEIGHBOURS = 5;
    size_t MAX_LINKS = 3;
    size_t NUMBER_OF_LOOPS = 3;

    auto neighbour_devices = createRoutingModules(NUMBER_OF_NEIGHBOURS);
    auto dest = std::make_shared<sim::RoutingModule>(sim::RoutingModule());
    auto source = std::make_shared<sim::RoutingModule>(sim::RoutingModule());

    std::mt19937 gen(RANDOM_SEED);
    std::uniform_int_distribution<> dis(1, MAX_LINKS);

    std::map<std::shared_ptr<sim::ILink>, int> number_of_inlink_appearances;
    std::map<std::shared_ptr<sim::ILink>, int> number_of_outlink_appearances;

    for (auto device : neighbour_devices) {
        auto inlink = std::make_shared<TestLink>(device, dest);
        auto outlink = std::make_shared<TestLink>(source, device);

        size_t links_to_add = dis(gen);
        for (size_t i = 0; i < links_to_add; ++i) {
            dest->add_inlink(inlink);
            source->add_outlink(outlink);
        }
        number_of_inlink_appearances[inlink] = 0;
        number_of_outlink_appearances[outlink] = 0;
    }

    auto outlinks = source->get_outlinks();
    sim::LoopIterator<
        std::set<std::weak_ptr<sim::ILink>,
                 std::owner_less<std::weak_ptr<sim::ILink>>>::iterator>
        outlink_it(outlinks.begin(), outlinks.end());

    for (size_t loop = 0; loop < NUMBER_OF_LOOPS; loop++) {
        for (size_t i = 0; i < NUMBER_OF_NEIGHBOURS; i++) {
            auto current_inlink = dest->next_inlink();
            number_of_inlink_appearances[current_inlink]++;
            auto current_outlink = *outlink_it++;
            number_of_outlink_appearances[current_outlink.lock()]++;
        }
    }
    for (auto appearances : number_of_inlink_appearances) {
        EXPECT_EQ(appearances.second, NUMBER_OF_LOOPS);
    }
    for (auto appearances : number_of_outlink_appearances) {
        EXPECT_EQ(appearances.second, NUMBER_OF_LOOPS);
    }
}

}  // namespace test
