#pragma once
#include <yaml-cpp/yaml.h>

#include "connection/flow/tcp/basic/basic_cc.hpp"
#include "connection/flow/tcp/i_tcp_cc_factory.hpp"


namespace test {

class TcpCCFactoryMock : public sim::ITcpCCFactory {
public:
    ~TcpCCFactoryMock() = default;
    TcpCCFactoryMock();
    std::unique_ptr<sim::ITcpCC> create_cc() final;
};
}  // namespace test