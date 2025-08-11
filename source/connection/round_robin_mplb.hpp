#pragma once

#include <memory>
#include <set>

#include "utils/loop_iterator.hpp"
#include "flow/i_flow.hpp"
#include "connection/i_mplb.hpp"

namespace sim {

class RoundRobinMPLB final : public IMPLB {
public:
    void add_flow(const std::shared_ptr<IFlow>& flow) override;
    void remove_flow(const std::shared_ptr<IFlow>& flow) override;

    std::shared_ptr<IFlow> select_flow() override;

    void notify_packet_confirmed(const std::shared_ptr<IFlow>&, FlowSample) override;
    void notify_packet_sent(const std::shared_ptr<IFlow>&, FlowSample) override;

private:
    using FlowPtr = std::shared_ptr<IFlow>;
    using Flows   = std::set<FlowPtr, std::owner_less<FlowPtr>>;

    Flows m_flows;
    LoopIterator<Flows::iterator> m_current_flow; 
};

}  // namespace sim
