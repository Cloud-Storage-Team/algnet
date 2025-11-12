#pragma once
#include <yaml-cpp/yaml.h>

#include "connection/flow/i_flow_factory.hpp"
#include "connection/mplb/i_new_mplb.hpp"

namespace test {
class FlowFactoryMock : public sim::INewFlowFactory {
public:
    ~FlowFactoryMock() = default;
    FlowFactoryMock(YAML::Node flow_node, Id conn_id,
                    const std::shared_ptr<sim::INewMPLB>& mplb,
                    const std::shared_ptr<sim::IHost> &sender,
                    const std::shared_ptr<sim::IHost> &receiver);
    std::shared_ptr<sim::INewFlow> create_flow() final;
private:
    sim::FlowContext m_context;
    std::weak_ptr<sim::INewMPLB> m_mplb;
};
}  // namespace test