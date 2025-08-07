#pragma once

#include <vector>
#include <memory>
#include <cstddef>

#include "connection/i_mplb.hpp"
#include "flow/i_flow.hpp"
#include "packet.hpp"

namespace sim {

class RoundRobinMPLB final : public IMPLB {
public:
    void add_flow(std::shared_ptr<IFlow> flow) override final;
    void remove_flow(const std::shared_ptr<IFlow>& flow) override final;
    std::shared_ptr<IFlow> select_flow() override final;
    void notify_ack_received(const std::shared_ptr<IFlow>& flow, const FlowSample flow_sample) override final;
    void notify_packet_sent(const std::shared_ptr<IFlow>& flow, const FlowSample flow_sample) override final;

private:
    std::vector<std::shared_ptr<IFlow>> m_flows;
    std::size_t m_next_index{0};
};

}  // namespace sim
