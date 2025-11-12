#include "cc_factory_mock.hpp"

namespace test {

ITcpCCFactoryMock::ITcpCCFactoryMock(
    [[maybe_unused]] sim::CCScope scope,
    [[maybe_unused]] const YAML::Node& cc_node) {
    // Mock implementation
}

std::unique_ptr<sim::ITcpCC> ITcpCCFactoryMock::create_cc() {
    return std::make_unique<sim::BasicCC>();
}

}  // namespace test
