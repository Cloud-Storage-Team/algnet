#include "flow_factory_mock.hpp"

#include "new_flow_mock.hpp"

namespace test {
FlowFactoryMock::FlowFactoryMock([[maybe_unused]] const YAML::Node& flow_node,
                                 [[maybe_unused]] Id conn_id,
                                 std::shared_ptr<sim::INewMPLB> mplb,
                                 std::shared_ptr<sim::IHost> sender,
                                 std::shared_ptr<sim::IHost> receiver)
    : m_mplb(mplb) {
    m_context.sender = sender;
    m_context.receiver = receiver;
}

std::shared_ptr<sim::INewFlow> FlowFactoryMock::create_flow() {
    return std::make_shared<NewFlowMock>("flow_m", m_context.sender.lock(),
                                         m_context.receiver.lock(),
                                         m_mplb.lock());
}
}  // namespace test