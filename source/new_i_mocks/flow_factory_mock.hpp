#pragma once
#include <yaml-cpp/yaml.h>

#include "connection/flow/i_flow_factory.hpp"
#include "connection/mplb/i_new_mplb.hpp"

namespace test {
class FlowFactoryMock : public sim::INewFlowFactory {
public:
    ~FlowFactoryMock() = default;
    FlowFactoryMock(const YAML::Node& flow_node,
                    std::shared_ptr<sim::IHost> sender,
                    std::shared_ptr<sim::IHost> receiver);
    std::shared_ptr<sim::INewFlow> create_flow(sim::INewMPLB& mplb) final;

private:
    sim::FlowContext m_context;
};
}  // namespace test