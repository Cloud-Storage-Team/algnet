#include "cc_factory_mock.hpp"

namespace test {

TcpCCFactoryMock::TcpCCFactoryMock() {
    // Mock implementation
}

std::unique_ptr<sim::ITcpCC> TcpCCFactoryMock::create_cc() {
    return std::make_unique<sim::BasicCC>();
}

}  // namespace test
