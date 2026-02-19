#include "parser/topology/topology_parser.hpp"

#include <gtest/gtest.h>

#include <filesystem>

namespace sim {
namespace test2 {

static std::filesystem::path project_root = PROJECT_ROOT_DIR;

static std::filesystem::path topologies_folder_path =
    project_root / "configuration_examples" / "topology_examples";

TEST(TopologyParsing, bus_topology) {
    std::filesystem::path bus_topology_path =
        topologies_folder_path / "bus_topology.yml";

    const ConfigNode config = load_file(bus_topology_path);

    Topology top = parse_topology(config);
}

}  // namespace test2
}  // namespace sim
