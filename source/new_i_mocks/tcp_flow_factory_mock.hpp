#pragma once
#include <yaml-cpp/yaml.h>

#include "connection/flow/i_flow_factory.hpp"
#include "connection/mplb/i_new_mplb.hpp"

namespace test {

struct TcpFlowFactoryParams {
    std::shared_ptr<sim::IHost> sender;
    std::shared_ptr<sim::IHost> receiver;
};

class TcpFlowFactoryMock : public sim::INewFlowFactory {
public:
    ~TcpFlowFactoryMock() = default;
    TcpFlowFactoryMock(TcpFlowFactoryParams& params);
    std::shared_ptr<sim::INewFlow> create_flow() final;

private:
    sim::FlowContext m_context;
    sim::INewMPLB::MPLBAckCallback m_ack_cb;
};
}  // namespace tests