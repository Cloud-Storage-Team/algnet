#include "tcp_flow_factory_mock.hpp"

#include "new_flow_mock.hpp"

namespace test {
TcpFlowFactoryMock::TcpFlowFactoryMock(TcpFlowFactoryParams& params) {
    m_context.sender = params.sender;
    m_context.receiver = params.receiver;
}

std::shared_ptr<sim::INewFlow> TcpFlowFactoryMock::create_flow() {
    auto flow = std::make_shared<NewFlowMock>("flow_m", m_context.sender.lock(),
                                         m_context.receiver.lock());
    flow->set_mplb_ack_cb(m_ack_cb);
    return flow;
}

}  // namespace test
