#pragma once
#include <yaml-cpp/yaml.h>
#include "connection/flow/tcp/i_tcp_cc_factory.hpp"
#include "connection/flow/tcp/basic/basic_cc.hpp"


namespace test {

class ITcpCCFactoryMock : public sim::ITcpCCFactory {
public:
    ~ITcpCCFactoryMock() = default;
    ITcpCCFactoryMock(sim::CCScope scope, const YAML::Node& cc_node);
    std::unique_ptr<sim::ITcpCC> create_cc() final;
};
}  // namespace sim