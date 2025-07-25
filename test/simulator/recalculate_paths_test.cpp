#include <gtest/gtest.h>

#include <filesystem>

#include "parser/parser.hpp"
#include "utils.hpp"
#include "logger/logger.hpp"

namespace test {

class RecalculatePaths : public testing::Test {
public:
    void TearDown() override {};
    void SetUp() override {};
};

static void check_pairwise_reachability(
    const sim::SimulatorVariant& simulator) {
    auto devices = std::visit([](const auto& sim) { return sim.get_devices(); },
                              simulator);
    for (auto src : devices) {
        for (auto dest : devices) {
            if (src != dest) {
                sim::Packet packet_to_dest = sim::Packet(SizeByte(0), nullptr, src->get_id(), dest->get_id());
                EXPECT_TRUE(check_reachability(src, packet_to_dest));
            }
        }
    }
}

void test_topology(std::filesystem::path topology_path) {
    sim::IdentifierFactory::get_instance().clear();
    sim::YamlParser parser;
    auto [simulator, sim_time] =
        parser.build_simulator_from_config(topology_path);
    std::visit([](auto& sim) { sim.recalculate_paths(); }, simulator);
    check_pairwise_reachability(simulator);
}

TEST_F(RecalculatePaths, TestTopologies) {
    std::filesystem::path current_file_path = __FILE__;
    std::filesystem::path topologies_dir_path =
        current_file_path.parent_path() / "topologies";
    for (const auto& entry :
         std::filesystem::directory_iterator(topologies_dir_path)) {
        test_topology(entry);
    }
}

}  // namespace test
